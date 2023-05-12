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
#include"SymbolTable.h"
// 3)auto type of return value function

llvm::Type* getType(TokenType type);

class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual llvm::Value* codegen(std::shared_ptr<SymbolTable> symbolTable = nullptr) = 0;
};


class VarDeclAST : public ASTNode {
public:
    VarDeclAST() = delete;
    VarDeclAST(const std::string& name, std::shared_ptr<ASTNode> value,TokenType type)
        : m_name(name), m_value(std::move(value)),m_type(type) {}

    llvm::Value* codegen(std::shared_ptr<SymbolTable> symbolTable = nullptr) override;
private:
    std::string m_name;
    std::shared_ptr<ASTNode> m_value;
    TokenType m_type;
};

class VarExprAST : public ASTNode {
public:
    VarExprAST() = delete;
    VarExprAST(std::string name) : m_name(std::move(name)) {}

    llvm::Value* codegen(std::shared_ptr<SymbolTable> symbolTable = nullptr) override;

private:
    std::string m_name;
};

class AssignExprAST : public ASTNode {
public:
    AssignExprAST() = delete;
    AssignExprAST(const std::string& varName, std::shared_ptr<ASTNode> val)
        : m_varName(varName), m_val(std::move(val)) {}

    llvm::Value* codegen(std::shared_ptr<SymbolTable> symbolTable = nullptr) override;

private:
    const std::string m_varName;
    std::shared_ptr<ASTNode> m_val;
};

template<typename T>
class LiteralExprAST : public ASTNode {
public:
    LiteralExprAST() = delete;
    LiteralExprAST(T value) : m_value(value) {}
    llvm::Value* codegen(std::shared_ptr<SymbolTable> symbolTable = nullptr) override;
private:
    T m_value;
};

class BinaryExprAST : public ASTNode {
public:
    BinaryExprAST() = delete;
    BinaryExprAST(TokenType op, std::shared_ptr<ASTNode> lhs, std::shared_ptr<ASTNode> rhs)
        : m_op(op), m_lhs(std::move(lhs)), m_rhs(std::move(rhs)) {}
    llvm::Value* codegen(std::shared_ptr<SymbolTable> symbolTable = nullptr) override;

private:
    TokenType m_op;
    std::shared_ptr<ASTNode> m_lhs, m_rhs;
};

class ConsoleOutputExprAST : public ASTNode {
public:
    ConsoleOutputExprAST() = delete;
    ConsoleOutputExprAST(std::shared_ptr<ASTNode> expr)
        : m_expr(std::move(expr)) {}
    llvm::Value* codegen(std::shared_ptr<SymbolTable> symbolTable = nullptr) override;
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
    llvm::Value* codegen(std::shared_ptr<SymbolTable> symbolTable = nullptr) override;

private:
    std::vector<std::shared_ptr<ASTNode>> m_stmts;
    std::shared_ptr<SymbolTable> m_symbolTable;
};

class FunctionAST : public ASTNode {
public:
    FunctionAST() = delete;
    FunctionAST(const std::string& name, TokenType retType,
        std::vector<std::pair<TokenType, std::string>> args,
        std::shared_ptr<BlockAST> body)
        : m_name(name), m_retType(retType), m_args(args), m_body(std::move(body)) {}

    llvm::Value* codegen(std::shared_ptr<SymbolTable> symbolTable = nullptr) override;

private:
    std::string m_name;
    TokenType m_retType;
    std::vector<std::pair<TokenType, std::string>> m_args;
    std::shared_ptr<BlockAST> m_body;
};
class ProtFunctionAST : public ASTNode {
public:
    ProtFunctionAST() = delete;
    ProtFunctionAST(const std::string& name, TokenType retType,
        std::vector<std::pair<TokenType, std::string>> args)
        : m_name(name), m_retType(retType), m_args(args){}

    llvm::Value* codegen(std::shared_ptr<SymbolTable> symbolTable = nullptr) override;

private:
    std::string m_name;
    TokenType m_retType;
    std::vector<std::pair<TokenType, std::string>> m_args;
};

class CallExprAST : public ASTNode {
public:
    CallExprAST() = delete;
    CallExprAST(const std::string& name, std::vector<std::shared_ptr<ASTNode>> args)
        : m_name(name), m_args(std::move(args)) {}

    llvm::Value* codegen(std::shared_ptr<SymbolTable> symbolTable = nullptr) override;

private:
    std::string m_name;
    std::vector<std::shared_ptr<ASTNode>> m_args;
};

class ReturnAST : public ASTNode
{
public:
    ReturnAST() = delete;
    ReturnAST(std::shared_ptr<ASTNode> retExpr)
        : m_retExpr(std::move(retExpr)){}
    llvm::Value* codegen(std::shared_ptr<SymbolTable> symbolTable = nullptr) override;
private:
    std::shared_ptr<ASTNode> m_retExpr;
};