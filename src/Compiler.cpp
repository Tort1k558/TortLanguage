#include"Compiler.h"
#include<fstream>
#include<filesystem>

#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/PassManager.h>
#include <llvm/IR/Verifier.h>
#include <llvm/LinkAllPasses.h>
#include <llvm/TargetParser/Host.h>
#include <llvm/Linker/Linker.h>
#include <llvm/BinaryFormat/ELF.h>
#include <lld/Common/LLVM.h>
#include <lld/Common/Driver.h>

#include"LLVMManager.h"

Compiler::Compiler(const std::string& pathToInputFile, OptimizationLevel optLevel)
	: m_optLevel(optLevel)
{
	std::filesystem::path filePath(pathToInputFile);
	m_fileName = filePath.filename().stem().string();
	m_directoryInputFile = filePath.parent_path().string();
	m_buildDirectory = m_directoryInputFile + "/build";
	std::filesystem::create_directory(m_buildDirectory);
}
void Compiler::compile()
{

	std::string code = readFile();

	//generate IR code
	auto lexer = std::make_shared<Lexer>(code);
	TokenStream tokenStream(std::move(lexer));
	Parser parser(tokenStream);
	parser.parse();


	//Write tokens to file
	std::string textTokens;
	while(tokenStream->type != TokenType::EndOfFile)
	{
		Token token = *tokenStream;
		textTokens += "TOKEN: [" + token.value + "] Type: " + g_nameTypes[static_cast<int>(token.type)] + '\n';
		tokenStream++;
	}
	writeFile(textTokens, m_buildDirectory + "/" + m_fileName + ".tk");
	generateIRFile();
	
	//optimize
	switch (m_optLevel)
	{
	case OptimizationLevel::O0:
		optimizeModule(llvm::OptimizationLevel::O0);
		break;
	case OptimizationLevel::O1:
		optimizeModule(llvm::OptimizationLevel::O1);
		break;
	case OptimizationLevel::O2:
		optimizeModule(llvm::OptimizationLevel::O2);
		break;
	case OptimizationLevel::O3:
		optimizeModule(llvm::OptimizationLevel::O3);
		break;
	default:
		break;
	}
	
	LLVMManager& manager = LLVMManager::getInstance();
	auto module = manager.getModule();


	if (llvm::verifyModule(*module,&llvm::outs()))
	{
		std::cerr << "ERROR::COMPILER::Module is not correct" << std::endl;
		return;
	}
	
	//initilize targetMachine
	llvm::InitializeAllTargetInfos();
	llvm::InitializeAllTargets();
	llvm::InitializeAllTargetMCs();
	llvm::InitializeAllAsmParsers();
	llvm::InitializeAllAsmPrinters();
	
	std::string targetTriple = llvm::sys::getDefaultTargetTriple();
	std::string error;
	const llvm::Target* target = llvm::TargetRegistry::lookupTarget(targetTriple, error);
	if (!target) {
		std::cerr << error << std::endl;
		return;
	}
	llvm::TargetOptions targetOptions;
	std::optional<llvm::Reloc::Model> relocModel;
	m_targetMachine = std::shared_ptr<llvm::TargetMachine>(target->createTargetMachine(targetTriple, llvm::sys::getHostCPUName().str(), "", targetOptions, relocModel));
	if (!m_targetMachine) {
		std::cerr << "ERROR::COMPILER::Failed to create target machine" << std::endl;
		return;
	}
	module->setDataLayout(m_targetMachine->createDataLayout());
	module->setTargetTriple(targetTriple);
	generateAsmFile();
	generateObjFile();
	generateExeFile();

}

void Compiler::generateExeFile()
{
	std::string pathToObjFile = m_buildDirectory + "/" + m_fileName + ".o";
	std::string pathToExeFile = m_buildDirectory + "/" + m_fileName + ".exe";

	llvm::raw_ostream& stdoutOS = llvm::outs();
	llvm::raw_ostream& stderrOS = llvm::errs();
	std::string outFile = "/out:" + pathToExeFile;
	const char* linkArgs[] = {
	  "lld::coff::link",
	  "/defaultlib:libcmt",
	  "-libpath:libs/windows/x64",
	  pathToObjFile.c_str(),
	  outFile.c_str()
	};

	bool exitEarly = false;
	bool disableOutput = false;
	bool success = lld::coff::link(linkArgs, stdoutOS, stderrOS, exitEarly, disableOutput);

	if (!success) {
		std::cerr << "ERROR::LINKER::Linking failed" << std::endl;
	}
	std::cout << "Executable file generated: "<< pathToExeFile << std::endl;
}
void Compiler::optimizeModule(llvm::OptimizationLevel optimize)
{
	LLVMManager& manager = LLVMManager::getInstance();
	auto module = manager.getModule();


	llvm::PassBuilder PB;

	llvm::LoopAnalysisManager LAM;
	llvm::FunctionAnalysisManager FAM;
	llvm::CGSCCAnalysisManager CGAM;
	llvm::ModuleAnalysisManager MAM;

	PB.registerModuleAnalyses(MAM);
	PB.registerCGSCCAnalyses(CGAM);
	PB.registerFunctionAnalyses(FAM);
	PB.registerLoopAnalyses(LAM);
	PB.crossRegisterProxies(LAM, FAM, CGAM, MAM);

	
	llvm::ModulePassManager MPM = PB.buildPerModuleDefaultPipeline(optimize);
	MPM.run(*module, MAM);
}

void Compiler::generateAsmFile()
{
	LLVMManager& manager = LLVMManager::getInstance();
	auto module = manager.getModule();

	std::string pathToAsmFile = m_buildDirectory + "/" + m_fileName + ".s";
	std::error_code EC;
	llvm::raw_fd_ostream dest(pathToAsmFile, EC, llvm::sys::fs::OF_None);

	if (EC) {
		std::cerr << "ERROR::COMPILER::Could not open file: " << EC.message() << std::endl;
		return;
	}

	llvm::legacy::PassManager pass;
	if (m_targetMachine->addPassesToEmitFile(pass, dest, nullptr, llvm::CGFT_AssemblyFile)) {
		std::cerr << "ERROR::COMPILER::TheTargetMachine can't emit a file of this type" << std::endl;
		return;
	}
	
	pass.run(*module);
	dest.flush();
	std::cout << "Assembler file generated: " << pathToAsmFile << std::endl;
}
void Compiler::generateObjFile()
{
	LLVMManager& manager = LLVMManager::getInstance();
	auto module = manager.getModule();
	
	std::string pathToObjFile = m_buildDirectory + "/" + m_fileName + ".o";
	std::error_code EC;
	llvm::raw_fd_ostream dest(pathToObjFile, EC, llvm::sys::fs::OF_None);

	if (EC) {
		std::cerr << "ERROR::COMPILER::Could not open file: " << EC.message() << std::endl;
		return;
	}

	llvm::legacy::PassManager pass;
	if (m_targetMachine->addPassesToEmitFile(pass, dest, nullptr, llvm::CGFT_ObjectFile)) {
		std::cerr << "ERROR::COMPILER::TheTargetMachine can't emit a file of this type" << std::endl;
		return;
	}

	pass.run(*module);
	dest.flush();
	std::cout << "Object file generated: " << pathToObjFile << std::endl;
}

std::string Compiler::readFile()
{
	std::string filename = m_directoryInputFile + "/" + m_fileName + ".tt";
	std::ifstream file(filename);
	if (!file.is_open())
	{
		std::cerr << "Error opening input file: " << filename << std::endl;
		return "";
	}
	std::string code;
	std::string line;
	while (std::getline(file,line))
	{
		code += line + '\n';
	}
	file.close();
	return code;
}

void Compiler::generateIRFile() {
	std::string pathToIRFile = m_buildDirectory + "/" + m_fileName + ".ll";

	std::error_code errorCode;
	llvm::raw_fd_ostream output(pathToIRFile, errorCode);
	if (errorCode) {
		std::cerr << "Error opening output file: " << errorCode.message() << std::endl;
		return;
	}
	LLVMManager& manager = LLVMManager::getInstance();
	auto module = manager.getModule();

	module->print(output, nullptr);
	output.close();
	std::cout << "IR file generated: " << pathToIRFile << std::endl;
}

void Compiler::writeFile(std::string text,std::string pathTofile)
{
	std::ofstream file(pathTofile, std::ios::out | std::ios::trunc);
	if (!file.is_open())
	{
		std::cerr << "ERROR::COMPILER::Error opening output file: " << pathTofile << std::endl;
		return;
	}
	file << text;
	return;
}