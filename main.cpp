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