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
#include<fstream>
int main() {
    llvm::LLVMContext* context = new llvm::LLVMContext();
    llvm::Module* module = new llvm::Module("MyModule", *context);
    llvm::IRBuilder<>* builder = new llvm::IRBuilder<>(*context);
    

    //Function Sum
    std::vector<std::pair<TokenType, std::string>> sumArgs{ {TokenType::Double,"a"},{TokenType::Double,"b"}};
    
    auto sumAB = std::make_unique<BinaryExprAST>('+', std::make_unique<VariableExprAST>("a"), std::make_unique<VariableExprAST>("b"));
    auto retSum = std::make_unique<ReturnAST>(std::move(sumAB));
    auto sumBody = std::make_unique<BlockAST>();
    sumBody->addStatement(std::move(retSum));

    auto sumFunc = std::make_unique<FunctionAST>("sum", TokenType::Double, sumArgs, std::move(sumBody));
    sumFunc->codegen(context, builder, module);


    //Function main
    auto varX = std::make_unique<VarDeclAST<double>>("x",0.0,TokenType::Double);
    auto varY = std::make_unique<VarDeclAST<double>>("y",0.0,TokenType::Double);

    auto assignX = std::make_unique<AssignExprAST>("x", std::make_unique<NumberExprAST<double>>(10.0));
    auto assignY = std::make_unique<AssignExprAST>("y", std::make_unique<NumberExprAST<double>>(50.0));

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
    func->codegen(context, builder, module);

    module->print(llvm::outs(), nullptr);

    std::string error;



}