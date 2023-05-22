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

#include"Token.h"
#include"SymbolTable.h"
#include"SymbolTableManager.h"

llvm::Type* getType(TokenType type);

class ASTNode {
public:
    llvm::Type* llvmType = nullptr;
    virtual ~ASTNode() = default;
    virtual llvm::Value* codegen() = 0;
};

class LLVMValueAST : public ASTNode
{
public:
    LLVMValueAST() = delete;
    LLVMValueAST(llvm::Value* value) : m_value(value)
    {
        llvmType = value->getType();
    }
    llvm::Value* codegen() override;
private:
    llvm::Value* m_value;
};

class VarDeclAST : public ASTNode {
public:
    VarDeclAST() = delete;
    VarDeclAST(const std::string& name, std::shared_ptr<ASTNode> value,TokenType type)
        : m_name(name), m_value(std::move(value)),m_type(type) {
        auto symbolTable = SymbolTableManager::getInstance().getSymbolTable();
        symbolTable->addVarType(m_name, getType(type));
    }

    llvm::Value* codegen() override;
private:
    std::string m_name;
    std::shared_ptr<ASTNode> m_value;
    TokenType m_type;
};

class VarExprAST : public ASTNode {
public:
    VarExprAST() = delete;
    VarExprAST(std::string name) : m_name(std::move(name)) { llvmType = SymbolTableManager::getInstance().getSymbolTable()->getTypeVar(m_name); }

    llvm::Value* codegen() override;

private:
    std::string m_name;
};

class AssignExprAST : public ASTNode {
public:
    AssignExprAST() = delete;
    AssignExprAST(const std::string& varName, std::shared_ptr<ASTNode> val)
        : m_varName(varName), m_val(std::move(val)) {}

    llvm::Value* codegen() override;

private:
    const std::string m_varName;
    std::shared_ptr<ASTNode> m_val;
};

template<typename T>
class LiteralExprAST : public ASTNode {
public:
    LiteralExprAST() = delete;
    LiteralExprAST(T value) : m_value(value) {
        LLVMManager& manager = LLVMManager::getInstance();
        std::shared_ptr<llvm::LLVMContext> context = manager.getContext();

        if (std::is_same<T, double>::value)
        {
            llvmType = llvm::Type::getDoubleTy(*context);
        }
        else if (std::is_same<T, int>::value)
        {
            llvmType = llvm::Type::getInt32Ty(*context);
        }
        else if (std::is_same<T, bool>::value)
        {
            llvmType = llvm::Type::getInt1Ty(*context);
        }
    }
    llvm::Value* codegen() override;
private:
    T m_value;
};

class BinaryExprAST : public ASTNode {
public:
    BinaryExprAST() = delete;
    BinaryExprAST(TokenType op, std::shared_ptr<ASTNode> lhs, std::shared_ptr<ASTNode> rhs)
        : m_op(op), m_lhs(std::move(lhs)), m_rhs(std::move(rhs)) {
        llvmType = m_lhs->llvmType;
    }
    llvm::Value* codegen() override;

private:
    TokenType m_op;
    std::shared_ptr<ASTNode> m_lhs, m_rhs;
};

class ConsoleOutputExprAST : public ASTNode {
public:
    ConsoleOutputExprAST() = delete;
    ConsoleOutputExprAST(std::shared_ptr<ASTNode> expr)
        : m_expr(std::move(expr)) {}
    llvm::Value* codegen() override;
private:
    std::shared_ptr<ASTNode> m_expr;
};

class ReturnAST : public ASTNode
{
public:
    ReturnAST() = delete;
    ReturnAST(std::shared_ptr<ASTNode> retExpr)
        : m_returnExpr(std::move(retExpr)), m_returnBB(nullptr)
    {
        if (m_returnExpr)
        {
            llvmType = m_returnExpr->llvmType;
        }
        else
        {
            LLVMManager& manager = LLVMManager::getInstance();
            auto builder = manager.getBuilder();
            llvmType = builder->getVoidTy();
        }
    }
    void setReturnBB(llvm::BasicBlock* BB)
    {
        m_returnBB = BB;
    }
    void setReturnVar(llvm::AllocaInst* var)
    {
        m_returnVar = var;
    }
    llvm::Value* codegen() override;
private:
    std::shared_ptr<ASTNode> m_returnExpr;
    llvm::BasicBlock* m_returnBB;
    llvm::AllocaInst* m_returnVar;
};

class BlockAST : public ASTNode {
public:
    BlockAST(std::shared_ptr<SymbolTable> symbolTable)
        : m_symbolTable(std::make_shared<SymbolTable>())
    {
        m_symbolTable->extend(symbolTable.get());
    }

    void addStatement(std::shared_ptr<ASTNode> stmt) {
        m_stmts.push_back(std::move(stmt));
    }
    void extendSymbolTable(std::shared_ptr<SymbolTable> symbolTable)
    {
        m_symbolTable->extend(symbolTable.get());
    }

    std::vector<std::shared_ptr<ReturnAST>> getReturns();
    llvm::Value* codegen() override;
private:
    std::vector<std::shared_ptr<ASTNode>> m_stmts;
    std::shared_ptr<SymbolTable> m_symbolTable;
};

class FunctionAST : public ASTNode {
public:
    FunctionAST() = delete;
    FunctionAST(const std::string& name, llvm::Type* retType,
        std::vector<std::pair<TokenType, std::string>> args,
        std::shared_ptr<BlockAST> body)
        : m_name(name), m_returnType(retType), m_args(args), m_body(std::move(body))
    {
        LLVMManager& manager = LLVMManager::getInstance();
        auto context = manager.getContext();

        if (!m_returnType)
        {
            m_returns = m_body->getReturns();
            if (!m_returns.empty())
            {
                retType = m_returns[0]->llvmType;
                for (size_t i = 0; i < m_returns.size(); i++)
                {
                    if (m_returns[i]->llvmType != retType)
                    {
                        throw std::runtime_error("ERROR::The function cannot return different types of values");
                    }
                }
                m_returnType = retType;
            }
            else
            {
                m_returnType = llvm::Type::getVoidTy(*context);
            }
        }
        llvmType = m_returnType;
    }

    llvm::Value* codegen() override;

private:
    std::string m_name;
    llvm::Type* m_returnType;
    std::vector<std::shared_ptr<ReturnAST>> m_returns;
    std::vector<std::pair<TokenType, std::string>> m_args;
    std::shared_ptr<BlockAST> m_body;

};

class ProtFunctionAST : public ASTNode {
public:
    ProtFunctionAST() = delete;
    ProtFunctionAST(const std::string& name, llvm::Type* retType,
        std::vector<std::pair<TokenType, std::string>> args)
        : m_name(name), m_returnType(retType), m_args(args)
    {
        llvmType = m_returnType;
    }

    llvm::Value* codegen() override;

private:
    std::string m_name;
    llvm::Type* m_returnType;
    std::vector<std::pair<TokenType, std::string>> m_args;
};

class CallExprAST : public ASTNode {
public:
    CallExprAST() = delete;
    CallExprAST(const std::string& name, std::vector<std::shared_ptr<ASTNode>> args)
        : m_name(name), m_args(std::move(args)) 
    {
        auto symbolTable = SymbolTableManager::getInstance().getSymbolTable();
        llvmType = symbolTable->getFunctionReturnType(name);
    }

    llvm::Value* codegen() override;

private:
    std::string m_name;
    std::vector<std::shared_ptr<ASTNode>> m_args;
};

class IfAST : public ASTNode
{
public:
    IfAST() = delete;
    IfAST(std::shared_ptr<ASTNode> ifExpr,std::shared_ptr<BlockAST> ifBlock, std::shared_ptr<BlockAST> elseBlock = nullptr, std::vector<std::pair<std::shared_ptr<ASTNode>, std::shared_ptr<BlockAST>>> elseIfs = {})
        : m_ifExpr(ifExpr), m_ifBlock(ifBlock),m_elseBlock(elseBlock), m_elseIfs(elseIfs)
    {
        llvmType = ifExpr->llvmType;
    }
    std::shared_ptr<BlockAST> getIfBlock(){ return m_ifBlock; }
    std::shared_ptr<BlockAST> getElseBlock(){ return m_elseBlock; }
    std::vector<std::pair<std::shared_ptr<ASTNode>, std::shared_ptr<BlockAST>>> getElseIfs(){ return m_elseIfs; }
    llvm::Value* codegen() override;
private:
    std::shared_ptr<ASTNode> m_ifExpr;
    std::shared_ptr<BlockAST> m_ifBlock;
    std::shared_ptr<BlockAST> m_elseBlock;
    std::vector<std::pair<std::shared_ptr<ASTNode>,std::shared_ptr<BlockAST>>> m_elseIfs;

};

class CastAST : public ASTNode
{
public:
    CastAST() = delete;
    CastAST(llvm::Value* value, llvm::Type* type)
        : m_value(value),m_type(type)
    {
        llvmType = type;
    }
    llvm::Value* codegen() override;
private:
    llvm::Value* m_value;
    llvm::Type* m_type;
};

class GotoAST : public ASTNode
{
public:
    GotoAST() = delete;
    GotoAST(llvm::BasicBlock* gotoBB, llvm::BasicBlock* gotoElseBB = nullptr,llvm::Value* value = nullptr)
        : m_gotoBB(gotoBB), m_gotoelseBB(gotoElseBB),m_value(m_value)
    {
        llvmType = nullptr;
    }
    llvm::Value* codegen() override;
private:
    llvm::BasicBlock* m_gotoBB;
    llvm::BasicBlock* m_gotoelseBB;
    llvm::Value* m_value;
};