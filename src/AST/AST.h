#pragma once

#include<stdio.h>
#include<iostream>
#include<cstdio>

#include<llvm/IR/LLVMContext.h>
#include<llvm/IR/IRBuilder.h>
#include<llvm/IR/Value.h>
#include<llvm/IR/Type.h>
#include<llvm/IR/Module.h>
#include<llvm/IR/Function.h>
#include<llvm/IR/BasicBlock.h>

#include"../Token.h"
#include"../SymbolTable.h"
#include"../SymbolTableManager.h"
#include"../LLVMManager.h"

llvm::Type* getType(TokenType type);
llvm::Value* castType(llvm::Value* value, llvm::Type* type);

class ASTNode {
public:
    llvm::Type* llvmType = nullptr;
    llvm::Value* llvmValue = nullptr;
    virtual ~ASTNode() = default;
    virtual void codegen() = 0;
    virtual void doSemantic() = 0;
    virtual llvm::Value* getRValue()
    {
        if (!llvmValue)
        {
            throw std::runtime_error("The expression has no value!");
        }
        return llvmValue;
    }
    virtual llvm::Value* getLValue()
    {
        LLVMManager& manager = LLVMManager::getInstance();
        auto builder = manager.getBuilder();
        if (!llvmValue)
        {
            throw std::runtime_error("The expression has no value!");
        }
        llvm::AllocaInst* allocaInst = llvm::dyn_cast<llvm::AllocaInst>(llvmValue);
        if (allocaInst)
        {
            llvm::Type* varType = allocaInst->getAllocatedType();
            if (varType->isArrayTy())
            {
                llvm::Value* zero = llvm::ConstantInt::get(builder->getInt64Ty(), 0);
                return builder->CreateGEP(varType, llvmValue, {zero, zero}, "ptrtoelementarray", true);
            }
            return builder->CreateLoad(varType, allocaInst);
        }
        return llvmValue;
    }
    void setName(std::string name)
    {
        m_name = name;
    }
    std::string getName()
    {
        return m_name;
    }
protected:
    std::string m_name;
};