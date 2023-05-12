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
        R"(
        def sum(int a,int b)
        {
            return a+b;
        }
    
        def double div(double a,double b);

        def main()
        {
            int a = 10,b =5;
            print a;
            print b;
            b = sum(5,8);
            print a + b * 10;
            double c = div(5.0,2.0);
            print c;
            return c;
        }
        def div(double a,double b)
        {
            return a / b ;
        })";

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