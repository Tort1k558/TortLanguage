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

// 2)own symbolTable for each BlockAST
// 3)auto type of return value function

llvm::Type* getType(llvm::LLVMContext* context, TokenType type);

class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual llvm::Value* codegen() = 0;
};

template<typename T>
class VarDeclAST : public ASTNode {
public:
    VarDeclAST() = delete;
    VarDeclAST(const std::string& name, T value,TokenType type)
        : m_name(name), m_value(value),m_type(type) {}

    llvm::Value* codegen() override;
private:
    std::string m_name;
    T m_value;
    TokenType m_type;
};

class VariableExprAST : public ASTNode {
public:
    VariableExprAST() = delete;
    VariableExprAST(std::string name) : m_name(std::move(name)) {}

    llvm::Value* codegen() override;

private:
    std::string m_name;
};

class AssignExprAST : public ASTNode {
public:
    AssignExprAST() = delete;
    AssignExprAST(const std::string& varName, std::unique_ptr<ASTNode> val)
        : m_varName(varName), m_val(std::move(val)) {}

    llvm::Value* codegen() override;

private:
    const std::string m_varName;
    std::unique_ptr<ASTNode> m_val;
};

template<typename T>
class NumberExprAST : public ASTNode {
public:
    NumberExprAST() = delete;
    NumberExprAST(T value) : m_value(value) {}
    llvm::Value* codegen() override;
private:
    T m_value;
};

class BinaryExprAST : public ASTNode {
public:
    BinaryExprAST() = delete;
    BinaryExprAST(TokenType op, std::unique_ptr<ASTNode> lhs, std::unique_ptr<ASTNode> rhs)
        : m_op(op), m_lhs(std::move(lhs)), m_rhs(std::move(rhs)) {}
    llvm::Value* codegen() override;

private:
    TokenType m_op;
    std::unique_ptr<ASTNode> m_lhs, m_rhs;
};

class ConsoleOutputExprAST : public ASTNode {
public:
    ConsoleOutputExprAST() = delete;
    ConsoleOutputExprAST(std::unique_ptr<ASTNode> expr)
        : m_expr(std::move(expr)) {}
    llvm::Value* codegen() override;
private:
    std::unique_ptr<ASTNode> m_expr;
};

class BlockAST : public ASTNode {
public:
    BlockAST() = default;

    void addStatement(std::unique_ptr<ASTNode> stmt) {
        m_stmts.push_back(std::move(stmt));
    }

    llvm::Value* codegen() override;

private:
    std::vector<std::unique_ptr<ASTNode>> m_stmts;
};

class FunctionAST : public ASTNode {
public:
    FunctionAST() = delete;
    FunctionAST(const std::string& name, TokenType retType,
        std::vector<std::pair<TokenType, std::string>> args,
        std::unique_ptr<BlockAST> body)
        : m_name(name), m_retType(retType), m_args(args), m_body(std::move(body)) {}

    llvm::Value* codegen() override;

private:
    std::string m_name;
    TokenType m_retType;
    std::vector<std::pair<TokenType, std::string>> m_args;
    std::unique_ptr<BlockAST> m_body;
};

class CallExprAST : public ASTNode {
public:
    CallExprAST() = delete;
    CallExprAST(const std::string& name, std::vector<std::unique_ptr<ASTNode>> args)
        : m_name(name), m_args(std::move(args)) {}

    llvm::Value* codegen() override;

private:
    std::string m_name;
    std::vector<std::unique_ptr<ASTNode>> m_args;
};

class ReturnAST : public ASTNode
{
public:
    ReturnAST() = delete;
    ReturnAST(std::unique_ptr<ASTNode> retExpr)
        : m_retExpr(std::move(retExpr)){}
    llvm::Value* codegen() override;
private:
    std::unique_ptr<ASTNode> m_retExpr;
};