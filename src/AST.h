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
    LLVMValueAST(llvm::Value* value) : m_value(value){}
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
    std::shared_ptr<ASTNode> getReturn();
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
        : m_name(name), m_retType(retType), m_args(args), m_body(std::move(body)) {}

    llvm::Value* codegen() override;

private:
    std::string m_name;
    llvm::Type* m_retType;
    std::vector<std::pair<TokenType, std::string>> m_args;
    std::shared_ptr<BlockAST> m_body;
};

class ProtFunctionAST : public ASTNode {
public:
    ProtFunctionAST() = delete;
    ProtFunctionAST(const std::string& name, llvm::Type* retType,
        std::vector<std::pair<TokenType, std::string>> args)
        : m_name(name), m_retType(retType), m_args(args){}

    llvm::Value* codegen() override;

private:
    std::string m_name;
    llvm::Type* m_retType;
    std::vector<std::pair<TokenType, std::string>> m_args;
};

class CallExprAST : public ASTNode {
public:
    CallExprAST() = delete;
    CallExprAST(const std::string& name, std::vector<std::shared_ptr<ASTNode>> args)
        : m_name(name), m_args(std::move(args)) { }

    llvm::Value* codegen() override;

private:
    std::string m_name;
    std::vector<std::shared_ptr<ASTNode>> m_args;
};

class ReturnAST : public ASTNode
{
public:
    ReturnAST() = delete;
    ReturnAST(std::shared_ptr<ASTNode> retExpr)
        : m_retExpr(std::move(retExpr)) {
        if (m_retExpr)
        {
            llvmType = m_retExpr->llvmType;
        }
    }
    llvm::Value* codegen() override;
private:
    std::shared_ptr<ASTNode> m_retExpr;
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