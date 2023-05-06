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

    auto var = std::make_unique<VarDeclAST<int>>("x",0,TokenType::Int);
    auto assign = std::make_unique<AssignExprAST>("x", std::make_unique<NumberExprAST<int>>(10));
    auto console_output = std::make_unique<ConsoleOutputExprAST>(std::make_unique<VariableExprAST>("x"));
    auto block = std::make_unique<BlockAST>();
    block->addStatement(std::move(var));
    block->addStatement(std::move(assign));
    block->addStatement(std::move(console_output));
    auto func = FunctionAST("main", TokenType::Bool, {}, std::move(block));
    func.codegen(context, builder, module);
    module->print(llvm::outs(), nullptr);

    std::string error;



}