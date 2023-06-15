#pragma once
#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<memory>
#include<map>


#include"src/Lexer.h"
#include"src/TokenStream.h"
#include"src/Parser.h"
#include"src/LLVMManager.h"
#include"src/Compiler.h"

int main(int argc, char* argv[]) {
    Compiler compiler;

	if (argc < 2)
	{
		std::cerr << "You must specify the parameters!" << std::endl;
		return 0;
	}

	for (size_t i = 0; i < argc; i++)
	{
		if (i == 0)
		{
			continue;
		}
		std::string parameter(argv[i]);
		if (parameter == "-O0")
		{
			compiler.setOptimizationLevel(OptimizationLevel::O0);
		}
		else if (parameter == "-O1")
		{
			compiler.setOptimizationLevel(OptimizationLevel::O1);
		}
		else if (parameter == "-O2")
		{
			compiler.setOptimizationLevel(OptimizationLevel::O2);
		}
		else if (parameter == "-O3")
		{
			compiler.setOptimizationLevel(OptimizationLevel::O3);
		}
		else if (parameter == "-input")
		{
			i++;
			if (i >= argc)
			{
				std::cerr << "You must specify the path to input file!" << std::endl;
				return 0;
			}
			compiler.setInputFile(argv[i]);
		}
		else
		{
			std::cerr << "Unknown parameter!" << std::endl;
			return 0;
		}
	}
	try
	{
		compiler.compile();
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

    return 0;
}