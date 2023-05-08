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
#include<fstream>
int main() {
    LLVMManager& manager = LLVMManager::getInstance();
    std::shared_ptr<llvm::Module> module = manager.getModule();
    
    

    //Function Sum
    std::vector<std::pair<TokenType, std::string>> sumArgs{ {TokenType::Int,"a"},{TokenType::Int,"b"}};
    
    auto sumAB = std::make_unique<BinaryExprAST>(TokenType::Mul, std::make_unique<VariableExprAST>("a"), std::make_unique<VariableExprAST>("b"));
    auto retSum = std::make_unique<ReturnAST>(std::move(sumAB));
    auto sumBody = std::make_unique<BlockAST>();
    sumBody->addStatement(std::move(retSum));

    auto sumFunc = std::make_unique<FunctionAST>("sum", TokenType::Int, sumArgs, std::move(sumBody));
    sumFunc->codegen();


    //Function main
    auto varX = std::make_unique<VarDeclAST<int>>("x",0,TokenType::Int);
    auto varY = std::make_unique<VarDeclAST<int>>("y",0,TokenType::Int);

    auto assignX = std::make_unique<AssignExprAST>("x", std::make_unique<NumberExprAST<int>>(10));
    auto assignY = std::make_unique<AssignExprAST>("y", std::make_unique<NumberExprAST<int>>(50));

    std::vector<std::unique_ptr<ASTNode>> vecArgs;
    vecArgs.push_back(std::make_unique<VariableExprAST>("x"));
    vecArgs.push_back(std::make_unique<VariableExprAST>("y"));
    auto console_output = std::make_unique<ConsoleOutputExprAST>(std::make_unique<CallExprAST>("sum", std::move(vecArgs)));

    auto block = std::make_unique<BlockAST>();
    block->addStatement(std::move(varX));
    block->addStatement(std::move(varY));
    block->addStatement(std::move(assignX));
    block->addStatement(std::move(assignY));
    block->addStatement(std::move(console_output));
    auto func = std::make_unique<FunctionAST>("main", TokenType::Bool, std::vector<std::pair<TokenType, std::string>>(), std::move(block));
    func->codegen();

    module->print(llvm::outs(), nullptr);

    std::string error;



}