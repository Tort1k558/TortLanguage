#pragma once
#include<string>

#include"Lexer.h"
#include"TokenStream.h"
#include"Parser.h"
#include"LLVMManager.h"
#include <llvm/Passes/PassBuilder.h>

enum class OptimizationLevel
{
	O0 = 0,
	O1,
	O2,
	O3,
};

class Compiler
{
public:
	Compiler(const std::string& pathToInputFile, OptimizationLevel optLevel = OptimizationLevel::O0);
	void compile();
private:
	std::string m_directoryInputFile;
	std::string m_pathToOutputFile;
	std::string m_fileName;
	OptimizationLevel m_optLevel;
	std::string readFile();
	void writeFile(std::string text, std::string pathTofile);
	void writeLLVMIRToFile();
	void generateObjFile(llvm::TargetMachine* targetMachine);
	void generateAsmFile(llvm::TargetMachine* targetMachine);
	void generateExeFile();
	void optimizeModule(llvm::OptimizationLevel optimize);
};