#pragma once
#include<string>

#include"Lexer.h"
#include"TokenStream.h"
#include"Parser.h"
#include"LLVMManager.h"
#include <llvm/Passes/PassBuilder.h>

class Compiler
{
public:
	Compiler(const std::string& pathToInputFile);
	void compile();
private:
	std::string m_directoryInputFile;
	std::string m_pathToOutputFile;
	std::string m_fileName;

	std::string readFile();
	void writeFile(std::string text, std::string pathTofile);
	void writeLLVMIRToFile();
	void generateObjFile();
	void generateAsmFile();
	void generateExeFile();
	void optimizeModule(llvm::OptimizationLevel optimize);
};