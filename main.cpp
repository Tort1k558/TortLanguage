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
#include"src/MyAST.h"
#include<fstream>

int main() {
    llvm::LLVMContext* context = new llvm::LLVMContext();
    llvm::Module* module = new llvm::Module("MyModule", *context);
    llvm::IRBuilder<>* builder = new llvm::IRBuilder<>(*context);

    
    auto main_func = llvm::Function::Create(
        llvm::FunctionType::get(builder->getInt32Ty(), false),
        llvm::Function::ExternalLinkage, "main", module);
    auto main_block = llvm::BasicBlock::Create(*context, "entry", main_func);
    builder->SetInsertPoint(main_block);

    auto var = std::make_unique<VarDeclAST>("x",0.0);
    auto assign = std::make_unique<AssignExprAST>("x", std::make_unique<NumberExprAST>(15.0).get());
    auto console_output = std::make_unique<ConsoleOutputExprAST>(std::make_unique<VariableExprAST>("x"));

    auto block = std::make_unique<BlockAST>();
    block->addStatement(var.get());
    block->addStatement(assign.get());
    block->addStatement(console_output.get());
    block->codegen(context, builder, module);

    builder->CreateRet(std::make_unique<VariableExprAST>("x")->codegen(context, builder, module));
    module->print(llvm::outs(), nullptr);

    std::string error;



}