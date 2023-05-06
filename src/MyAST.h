#pragma once

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Module.h>
#include<stdio.h>
#include<iostream>
#include <cstdio>
#include"Token.h"
#include"externs.h"

class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual llvm::Value* codegen(llvm::LLVMContext* context, llvm::IRBuilder<>* builder, llvm::Module* modul) = 0;
};

class VarDeclAST : public ASTNode {
public:
    VarDeclAST(const std::string& name, double value)
        : m_name(name), m_value(value) {}

    llvm::Value* codegen(llvm::LLVMContext* context, llvm::IRBuilder<>* builder, llvm::Module* modul) override
    {
        llvm::AllocaInst* alloca = builder->CreateAlloca(llvm::Type::getDoubleTy(*context), nullptr, m_name.c_str());

        llvm::Value* val = llvm::ConstantFP::get(*context, llvm::APFloat(m_value));
        builder->CreateStore(val, alloca);

        Table::symbol_table[m_name] = alloca;

        return alloca;
    }
private:
    std::string m_name;
    double m_value;
};

class VariableExprAST : public ASTNode {
public:
    VariableExprAST(std::string name) : m_name(std::move(name)) {}

    llvm::Value* codegen(llvm::LLVMContext* context, llvm::IRBuilder<>* builder, llvm::Module* modul) override
    {
        llvm::AllocaInst* inst = Table::symbol_table[m_name];
        llvm::Value* var = builder->CreateLoad(inst->getAllocatedType(), inst);
        if (!var) {
            std::cerr << "Unknown variable name: " << m_name << std::endl;
            return nullptr;
        }
        return var;
    }

private:
    std::string m_name;
};
class AssignExprAST : public ASTNode {
public:
    AssignExprAST(const std::string& varName, ASTNode* val)
        : m_varName(varName), m_val(val) {}

    llvm::Value* codegen(llvm::LLVMContext* context, llvm::IRBuilder<>* builder, llvm::Module* modul) override{
        llvm::AllocaInst* var = Table::symbol_table[m_varName];
        if (!var) {
            std::cerr << "Var is not defined" << std::endl;
        }
        llvm::Value* val = m_val->codegen(context, builder, modul);
        if (!val) {
            return nullptr;
        }
        builder->CreateStore(val, var);
    }

private:
    const std::string m_varName;
    ASTNode* m_val;
};
class NumberExprAST : public ASTNode {
public:
    NumberExprAST(double value) : m_value(value) {}
    llvm::Value* codegen(llvm::LLVMContext* context, llvm::IRBuilder<>* builder, llvm::Module* modul) override
    {
        return llvm::ConstantFP::get(*context, llvm::APFloat(m_value));
    }

private:
    double m_value;
};

class BinaryExprAST : public ASTNode {
public:
    BinaryExprAST(char op, std::unique_ptr<ASTNode> lhs,
        std::unique_ptr<ASTNode> rhs)
        : m_op(op), m_lhs(std::move(lhs)), m_rhs(std::move(rhs)) {}
    llvm::Value* codegen(llvm::LLVMContext* context, llvm::IRBuilder<>* builder, llvm::Module* modul) override
    {
        llvm::Value* lhs_val = m_lhs->codegen(context,builder, modul);
        llvm::Value* rhs_val = m_rhs->codegen(context, builder,modul);
        if (!lhs_val || !rhs_val) {
            return nullptr;
        }

        switch (m_op) {
        case '+':
            return builder->CreateFAdd(lhs_val, rhs_val, "addtmp");
        case '-':
            return builder->CreateFSub(lhs_val, rhs_val, "subtmp");
        case '*':
            return builder->CreateFMul(lhs_val, rhs_val, "multmp");
        case '/':
            return builder->CreateFDiv(lhs_val, rhs_val, "divtmp");
        default:
            std::cerr << "Invalid binary operator: " << m_op << std::endl;
            return nullptr;
        }
    }

private:
    char m_op;
    std::unique_ptr<ASTNode> m_lhs, m_rhs;
};

class ConsoleOutputExprAST : public ASTNode {
public:
    ConsoleOutputExprAST(std::unique_ptr<ASTNode> expr)
        : m_expr(std::move(expr)) {}
    llvm::Value* codegen(llvm::LLVMContext* context, llvm::IRBuilder<>* builder, llvm::Module* modul) override
    {
        static llvm::Function* printFunc;

        if (!printFunc)
        {
            llvm::FunctionType* printFuncType = llvm::FunctionType::get(llvm::IntegerType::getInt32Ty(*context), llvm::PointerType::get(llvm::IntegerType::getInt8Ty(*context), 0), true);
            printFunc = llvm::Function::Create(printFuncType, llvm::Function::ExternalLinkage, "printf", modul);
        }
        if (!printFunc) {
            std::cerr << "Printf function not found" << std::endl;
            return nullptr;
        }

        llvm::Value* val = m_expr->codegen(context, builder, modul);
        if (!val) {
            return nullptr;
        }

        std::string format_str;
        llvm::Type* i = val->getType();
        if (i->isFloatTy() or i->isDoubleTy()) {
            format_str = "%f\n";
        }

        std::vector<llvm::Value*> printf_args;
        llvm::Value* format_str_const = builder->CreateGlobalStringPtr(format_str);
        printf_args.push_back(format_str_const);
        printf_args.push_back(val);
        

        return builder->CreateCall(printFunc, llvm::ArrayRef<llvm::Value* >(printf_args));
    }
private:
    std::unique_ptr<ASTNode> m_expr;
};

class BlockAST : public ASTNode {
public:
    BlockAST() = default;

    void addStatement(ASTNode* stmt) {
        m_stmts.push_back(stmt);
    }

    llvm::Value* codegen(llvm::LLVMContext* context, llvm::IRBuilder<>* builder, llvm::Module* modul) override {
        llvm::Value* lastVal = nullptr;
        for (ASTNode* stmt : m_stmts) {
            lastVal = stmt->codegen(context, builder, modul);
        }
        return lastVal;
    }

private:
    std::vector<ASTNode*> m_stmts;
};
