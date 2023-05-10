#pragma once
#include <iostream>
#include <string>
#include <vector>
#include<memory>
#include<map>

#include <llvm/ADT/APInt.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>

#include"src/Token.h"
#include"src/Lexer.h"
#include"src/TokenStream.h"
#include"src/Parser.h"
#include"src/AST.h"
#include"src/LLVMManager.h"
#include"src/SymbolTable.h"
#include<fstream>
int main() {
    std::string m_input =
        "def int sum(int f,int e){return f+e;}"
        "def int main(){int a; int b; b = 10; a = 25*4 / (5);int c = b > a; sum(a,b); return a;}"
        "def double div(double a,double b){return a/b;}";

    auto lexer = std::make_shared<Lexer>(m_input);
    TokenStream tokenStream(std::move(lexer));
    Parser parser(&tokenStream);
    parser.parse();
    while (tokenStream->type != TokenType::EndOfFile)
    {
        Token token = *tokenStream;
        std::cout << "TOKEN: [" + token.value + "] Type: " + g_nameTypes[static_cast<int>(token.type)] << std::endl;
        tokenStream++;
    }
    

    LLVMManager& manager = LLVMManager::getInstance();
    std::shared_ptr<llvm::Module> module = manager.getModule();

    module->print(llvm::outs(), nullptr);

    std::string error;



}