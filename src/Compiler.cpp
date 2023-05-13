#include"Compiler.h"
#include<fstream>
#include<filesystem>
#include"LLVMManager.h"

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

Compiler::Compiler(const std::string& pathToInputFile, OptimizationLevel optLevel = OptimizationLevel::O0)
	: m_optLevel(optLevel)
{
	std::filesystem::path filePath(pathToInputFile);
	m_fileName = filePath.filename().stem().string();
	m_directoryInputFile = filePath.parent_path().string();
}
void Compiler::compile()
{
	std::string code = readFile();

	auto lexer = std::make_shared<Lexer>(code);
	TokenStream tokenStream(std::move(lexer));
	Parser parser(tokenStream);
	parser.parse();

	std::string textTokens;
	while(tokenStream->type != TokenType::EndOfFile)
	{
		Token token = *tokenStream;
		textTokens += "TOKEN: [" + token.value + "] Type: " + g_nameTypes[static_cast<int>(token.type)] + '\n';
		tokenStream++;
	}

	writeFile(textTokens, m_directoryInputFile + "\\" + m_fileName + ".tk");
	
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

	//initilize targetMachine
	LLVMManager& manager = LLVMManager::getInstance();
	auto module = manager.getModule();

	if (llvm::verifyModule(*module))
	{
		std::cerr << "ERROR::COMPILER::Module is not correct" << std::endl;
		return;
	}
	
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
	llvm::TargetMachine* targetMachine = target->createTargetMachine(targetTriple, llvm::sys::getHostCPUName().str(), "", targetOptions, relocModel);
	if (!targetMachine) {
		std::cerr << "ERROR::COMPILER::Failed to create target machine" << std::endl;
		return;
	}
	module->setDataLayout(targetMachine->createDataLayout());

	writeLLVMIRToFile();
	generateAsmFile(targetMachine);
	generateObjFile(targetMachine);
	generateExeFile();
}

void Compiler::generateExeFile()
{
	std::string objFilePath = m_directoryInputFile + "\\" + m_fileName + ".obj";
	std::string exeFilePath = m_directoryInputFile + "\\" + m_fileName + ".exe";

	std::string command = "lld-link " + objFilePath + " -out:" + exeFilePath + " /DEFAULTLIB:libcmt";

	int result = std::system(command.c_str());
	if (result != 0)
	{
		std::cerr << "ERROR::COMPILER::Failed to create executable file: " << exeFilePath << std::endl;
		return;
	}

	std::cout << "Executable file generated: " << exeFilePath << std::endl;
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
void Compiler::generateAsmFile(llvm::TargetMachine* targetMachine)
{
	LLVMManager& manager = LLVMManager::getInstance();
	auto module = manager.getModule();

	auto Filename = m_directoryInputFile + "\\" + m_fileName + ".s";
	std::error_code EC;
	llvm::raw_fd_ostream dest(Filename, EC, llvm::sys::fs::OF_None);

	if (EC) {
		std::cerr << "ERROR::COMPILER::Could not open file: " << EC.message() << std::endl;
		return;
	}
	llvm::legacy::PassManager pass;
	auto FileType = llvm::CGFT_AssemblyFile;
	if (targetMachine->addPassesToEmitFile(pass, dest, nullptr, FileType)) {
		std::cerr << "ERROR::COMPILER::TheTargetMachine can't emit a file of this type" << std::endl;
		return;
	}
	
	pass.run(*module);
	dest.flush();
	llvm::outs() << "Assembler file generated: " << Filename << "\n";
}
void Compiler::generateObjFile(llvm::TargetMachine* targetMachine)
{
	LLVMManager& manager = LLVMManager::getInstance();
	auto module = manager.getModule();

	auto Filename = m_directoryInputFile + "\\" + m_fileName + ".obj";
	std::error_code EC;
	llvm::raw_fd_ostream dest(Filename, EC, llvm::sys::fs::OF_None);

	if (EC) {
		std::cerr << "ERROR::COMPILER::Could not open file: " << EC.message() << std::endl;
		return;
	}
	llvm::legacy::PassManager pass;
	auto FileType = llvm::CGFT_ObjectFile;

	if (targetMachine->addPassesToEmitFile(pass, dest, nullptr, FileType)) {
		std::cerr << "ERROR::COMPILER::TheTargetMachine can't emit a file of this type" << std::endl;
		return;
	}

	pass.run(*module);
	dest.flush();
	llvm::outs() << "Object file generated: " << Filename << "\n";
}

std::string Compiler::readFile()
{
	std::ifstream file(m_directoryInputFile+"\\"+ m_fileName + ".tt");
	if (!file.is_open())
	{
		std::cerr << "Error opening input file: " << m_directoryInputFile + "\\" + m_fileName + ".tt" << std::endl;
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
void Compiler::writeLLVMIRToFile() {
	std::error_code errorCode;
	llvm::raw_fd_ostream output(m_directoryInputFile + "\\" + m_fileName +".ll", errorCode);
	LLVMManager& manager = LLVMManager::getInstance();
	auto module = manager.getModule();
	if (errorCode) {
		std::cerr << "Error opening output file: " << errorCode.message() << std::endl;
		return;
	}
	module->print(output, nullptr);
	output.close();
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