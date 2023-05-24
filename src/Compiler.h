#pragma once
#include<string>

#include"Lexer.h"
#include"TokenStream.h"
#include"Parser.h"
#include"LLVMManager.h"
#include <llvm/Passes/PassBuilder.h>

enum class OptimizationLevel
{
	None = 0,
	O0,
	O1,
	O2,
	O3,
};

class Compiler
{
public:
	Compiler();
	void compile();
	void setInputFile(const std::string& pathToInputFile);
	void setOptimizationLevel(OptimizationLevel optLevel);
private:
	std::string m_directoryInputFile;
	std::string m_pathToOutputFile;
	std::string m_fileName;
	std::string m_buildDirectory;
	std::shared_ptr<llvm::TargetMachine> m_targetMachine;
	OptimizationLevel m_optLevel;
	std::string readFile();
	void writeFile(std::string text, std::string pathTofile);
	void generateIRFile();
	void generateObjFile();
	void generateAsmFile();
	void generateExeFile();
	void optimizeModule(llvm::OptimizationLevel optimize);
};