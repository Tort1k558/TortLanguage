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
        "def int sum(int f,int e){}"
        "def int main(){int a; int b;sum(5,10);}"
        "def int div(){}";

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
    

    std::shared_ptr<SymbolTable> globalSymbolTable = std::make_shared<SymbolTable>();
    //Function Sum
    std::vector<std::pair<TokenType, std::string>> sumArgs{ {TokenType::Int,"a"},{TokenType::Int,"b"}};
    
    auto sumAB = std::make_shared<BinaryExprAST>(TokenType::Mul, std::make_shared<VarExprAST>("a"), std::make_shared<VarExprAST>("b"));
    auto retSum = std::make_shared<ReturnAST>(std::move(sumAB));
    auto sumBody = std::make_shared<BlockAST>(globalSymbolTable);
    sumBody->addStatement(std::move(retSum));

    auto sumFunc = std::make_shared<FunctionAST>("sum", TokenType::Int, sumArgs, std::move(sumBody));
    sumFunc->codegen(globalSymbolTable);


    //Function main
    auto varX = std::make_shared<VarDeclAST>("x",0,TokenType::Int);
    auto varY = std::make_shared<VarDeclAST>("y",0,TokenType::Int);

    auto assignX = std::make_shared<AssignExprAST>("x", std::make_shared<LiteralExprAST<int>>(10));
    auto assignY = std::make_shared<AssignExprAST>("y", std::make_shared<LiteralExprAST<int>>(50));

    std::vector<std::shared_ptr<ASTNode>> vecArgs;
    vecArgs.push_back(std::make_shared<VarExprAST>("x"));
    vecArgs.push_back(std::make_shared<VarExprAST>("y"));
    auto console_output = std::make_shared<ConsoleOutputExprAST>(std::make_shared<CallExprAST>("sum", std::move(vecArgs)));

    auto block = std::make_shared<BlockAST>(globalSymbolTable);
    block->addStatement(std::move(varX));
    block->addStatement(std::move(varY));
    block->addStatement(std::move(assignX));
    block->addStatement(std::move(assignY));
    block->addStatement(std::move(console_output));
    auto func = std::make_shared<FunctionAST>("main", TokenType::Bool, std::vector<std::pair<TokenType, std::string>>(), std::move(block));
    func->codegen(globalSymbolTable);

    LLVMManager& manager = LLVMManager::getInstance();
    std::shared_ptr<llvm::Module> module = manager.getModule();

    module->print(llvm::outs(), nullptr);

    std::string error;



}