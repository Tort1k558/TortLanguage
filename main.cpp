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

//TODO
// &&,||,
//Не правильно реализовал
int main() {
    Compiler compiler("example/main.tt");
    compiler.compile();

    return 0;
}