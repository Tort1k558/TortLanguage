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


int main() {
    Compiler compiler("example/main.tt");
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