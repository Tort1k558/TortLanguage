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
    virtual void doSemantic() = 0;
};

class LLVMValueAST : public ASTNode
{
public:
    LLVMValueAST() = delete;
    LLVMValueAST(llvm::Value* value) : m_value(value)
    {}
    void doSemantic() override
    {
        llvmType = m_value->getType();
    }
    llvm::Value* codegen() override;
private:
    llvm::Value* m_value;
};

class VarDeclAST : public ASTNode {
public:
    VarDeclAST() = delete;
    VarDeclAST(const std::string& name, std::shared_ptr<ASTNode> value, TokenType type)
        : m_name(name), m_value(value), m_type(type) {}
    void doSemantic() override
    {
        auto symbolTable = SymbolTableManager::getInstance().getSymbolTable();
        llvmType = getType(m_type);
        if (m_type == TokenType::Var)
        {
            if (!m_value)
            {
                throw std::runtime_error("ERROR::AST::The variable of type var must be initialized immediately");
            }
            m_value->doSemantic();
            llvmType = m_value->llvmType;
        }

        symbolTable->addVarType(m_name, llvmType);
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
    VarExprAST(std::string name) : m_name(std::move(name)) { }

    void doSemantic() override
    {
        llvmType = SymbolTableManager::getInstance().getSymbolTable()->getTypeVar(m_name);
    }
    llvm::Value* codegen() override;
private:
    std::string m_name;
    llvm::Value* m_location;
};

class AssignExprAST : public ASTNode {
public:
    AssignExprAST() = delete;
    AssignExprAST(const std::string& varName, std::shared_ptr<ASTNode> val)
        : m_varName(varName), m_val(std::move(val)) {}

    void doSemantic() override
    {

    }
    llvm::Value* codegen() override;

private:
    const std::string m_varName;
    std::shared_ptr<ASTNode> m_val;
};


class LiteralExprAST : public ASTNode {
public:
    LiteralExprAST() = delete;
    LiteralExprAST(const std::string& value, TokenType type) : m_value(value), m_type(type){}

    void doSemantic() override
    {
        LLVMManager& manager = LLVMManager::getInstance();
        auto context = manager.getContext();
        auto builder = manager.getBuilder();

        if (m_type == TokenType::DoubleLiteral)
        {
            llvmType = builder->getDoubleTy();
        }
        else if (m_type == TokenType::IntLiteral)
        {
            llvmType = builder->getInt32Ty();
        }
        else if (m_type == TokenType::TrueLiteral || m_type == TokenType::FalseLiteral)
        {
            llvmType = builder->getInt1Ty();
        }
        else if (m_type == TokenType::StringLiteral)
        {
            llvmType = builder->getInt8PtrTy();
        }
    }
    llvm::Value* codegen() override;
private:
    std::string m_value;
    TokenType m_type;
};

class BinaryExprAST : public ASTNode {
public:
    BinaryExprAST() = delete;
    BinaryExprAST(TokenType op, std::shared_ptr<ASTNode> lhs, std::shared_ptr<ASTNode> rhs)
        : m_op(op), m_lhs(std::move(lhs)), m_rhs(std::move(rhs)) {}
    void doSemantic() override
    {
        m_lhs->doSemantic();
        llvmType = m_lhs->llvmType;
    }
    llvm::Value* codegen() override;
private:
    TokenType m_op;
    std::shared_ptr<ASTNode> m_lhs, m_rhs;
};
class UnaryExprAST : public ASTNode {
public:
    UnaryExprAST() = delete;
    UnaryExprAST(TokenType op, const std::string& name,bool prefix = false)
        : m_op(op),m_name(name),m_prefix(prefix) {}
    void doSemantic() override
    {
        auto symbolTable = SymbolTableManager::getInstance().getSymbolTable();
        llvmType = symbolTable->getTypeVar(m_name);
    }
    llvm::Value* codegen() override;

private:
    TokenType m_op;
    std::string m_name;
    bool m_prefix;
};
class ConsoleOutputExprAST : public ASTNode {
public:
    ConsoleOutputExprAST() = delete;
    ConsoleOutputExprAST(std::shared_ptr<ASTNode> expr)
        : m_expr(std::move(expr)) {}
    void doSemantic() override
    {
        LLVMManager& manager = LLVMManager::getInstance();
        auto builder = manager.getBuilder();
        llvmType = builder->getVoidTy();
    }
    llvm::Value* codegen() override;
private:
    std::shared_ptr<ASTNode> m_expr;
};

class ReturnAST : public ASTNode
{
public:
    ReturnAST() = delete;
    ReturnAST(std::shared_ptr<ASTNode> retExpr)
        : m_returnExpr(std::move(retExpr)), m_returnBB(nullptr) {}
    void doSemantic() override
    {
        if (m_returnExpr)
        {
            m_returnExpr->doSemantic();
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
    std::shared_ptr<ASTNode> getReturnExpr()
    {
        return m_returnExpr;
    }
    llvm::Value* codegen() override;
private:
    std::shared_ptr<ASTNode> m_returnExpr;
    llvm::BasicBlock* m_returnBB;
    llvm::AllocaInst* m_returnVar;
};

class BreakAST : public ASTNode
{
public:
    BreakAST():
    m_nextBlock(nullptr) {}
    virtual void doSemantic()
    {

    }
    void setNextBlock(llvm::BasicBlock* block)
    {
        m_nextBlock = block;
    }
    llvm::Value* codegen() override;
private:
    llvm::BasicBlock* m_nextBlock;
};

class ContinueAST : public ASTNode
{
public:
    ContinueAST() :
        m_nextBlock(nullptr) {}
    virtual void doSemantic()
    {

    }
    void setNextBlock(llvm::BasicBlock* block)
    {
        m_nextBlock = block;
    }
    llvm::Value* codegen() override;
private:
    llvm::BasicBlock* m_nextBlock;
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
    std::vector<std::shared_ptr<BreakAST>> getBreaks();
    void doSemantic() override
    {
        for (const auto& stmt : m_stmts)
        {
            std::shared_ptr<ReturnAST> retAST = std::dynamic_pointer_cast<ReturnAST>(stmt);
            if (retAST)
            {
                continue;
            }
            stmt->doSemantic();
        }
    }
    llvm::Value* codegen() override;
private:
    std::vector<std::shared_ptr<ASTNode>> m_stmts;
    std::shared_ptr<SymbolTable> m_symbolTable;
};

class CallExprAST : public ASTNode {
public:
    CallExprAST() = delete;
    CallExprAST(const std::string& name, std::vector<std::shared_ptr<ASTNode>> args)
        : m_name(name), m_args(std::move(args)) {}
    virtual void doSemantic()
    {
        auto symbolTable = SymbolTableManager::getInstance().getSymbolTable();
        llvmType = symbolTable->getFunctionReturnType(m_name);
    }
    std::string getNameCallFunction()
    {
        return m_name;
    }
    llvm::Value* codegen() override;

private:
    std::string m_name;
    std::vector<std::shared_ptr<ASTNode>> m_args;
};

class FunctionAST : public ASTNode {
public:
    FunctionAST() = delete;
    FunctionAST(const std::string& name, TokenType retType,
        std::vector<std::pair<TokenType, std::string>> args,
        std::shared_ptr<BlockAST> body)
        : m_name(name), m_returnType(getType(retType)), m_args(args), m_body(std::move(body))
    {}
    void doSemantic() override
    {
        LLVMManager& manager = LLVMManager::getInstance();
        auto context = manager.getContext();
        m_body->doSemantic();

        if (!m_returnType)
        {
            m_returns = m_body->getReturns();
            if (!m_returns.empty())
            {
                std::vector<std::shared_ptr<ReturnAST>> returnRecursion;

                for (const auto& ret : m_returns)
                {
                    std::shared_ptr<CallExprAST> callExpr = std::dynamic_pointer_cast<CallExprAST>(ret->getReturnExpr());
                    if (callExpr)
                    {
                        if (callExpr->getNameCallFunction() == m_name)
                        {
                            returnRecursion.push_back(ret);
                            continue;
                        }
                    }
                    ret->doSemantic();
                    auto symbolTable = SymbolTableManager::getInstance().getSymbolTable();
                    symbolTable->addFunctionReturnType(m_name, ret->llvmType);
                }

                if (returnRecursion.size() == m_returns.size())
                {
                    throw std::runtime_error("ERROR::Could not determine the type of the returned function");
                }

                for (const auto& ret : returnRecursion)
                {
                    ret->doSemantic();
                }
                
                llvm::Type* retType = m_returns[0]->llvmType;
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
    ProtFunctionAST(const std::string& name, TokenType retType,
        std::vector<std::pair<TokenType, std::string>> args)
        : m_name(name), m_returnType(getType(retType)), m_args(args) {}
    void doSemantic() override
    {
        llvmType = m_returnType;
    }
    llvm::Value* codegen() override;

private:
    std::string m_name;
    llvm::Type* m_returnType;
    std::vector<std::pair<TokenType, std::string>> m_args;
};

class IfAST : public ASTNode
{
public:
    IfAST() = delete;
    IfAST(std::shared_ptr<ASTNode> ifExpr,std::shared_ptr<BlockAST> ifBlock, std::shared_ptr<BlockAST> elseBlock = nullptr, std::vector<std::pair<std::shared_ptr<ASTNode>, std::shared_ptr<BlockAST>>> elseIfs = {})
        : m_ifExpr(ifExpr), m_ifBlock(ifBlock),m_elseBlock(elseBlock), m_elseIfs(elseIfs)
    {}
    std::shared_ptr<BlockAST> getIfBlock(){ return m_ifBlock; }
    std::shared_ptr<BlockAST> getElseBlock(){ return m_elseBlock; }
    std::vector<std::pair<std::shared_ptr<ASTNode>, std::shared_ptr<BlockAST>>> getElseIfs(){ return m_elseIfs; }
    void doSemantic() override
    {
        m_ifExpr->doSemantic();
        m_ifBlock->doSemantic();
        for (const auto& block : m_elseIfs)
        {
            block.second->doSemantic();
        }
        if (m_elseBlock)
        {
            m_elseBlock->doSemantic();
        }
        llvmType = m_ifExpr->llvmType;
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
    CastAST(std::shared_ptr<ASTNode> value, TokenType type)
        : m_value(value), m_type(type) {}
    void doSemantic() override
    {
        llvmType = getType(m_type);
    }
    llvm::Value* codegen() override;
private:
    std::shared_ptr<ASTNode> m_value;
    TokenType m_type;
};

class GotoAST : public ASTNode
{
public:
    GotoAST() = delete;
    GotoAST(llvm::BasicBlock* gotoBB, llvm::BasicBlock* gotoElseBB = nullptr,llvm::Value* value = nullptr)
        : m_gotoBB(gotoBB), m_gotoelseBB(gotoElseBB),m_value(m_value) {}
    virtual void doSemantic()
    {
        
    }
    llvm::Value* codegen() override;
private:
    llvm::BasicBlock* m_gotoBB;
    llvm::BasicBlock* m_gotoelseBB;
    llvm::Value* m_value;
};

class WhileAST : public ASTNode
{
public:
    WhileAST() = delete;
    WhileAST(std::shared_ptr<ASTNode> whileExpr, std::shared_ptr<BlockAST> whileBlock)
        : m_whileExpr(whileExpr), m_whileBlock(whileBlock) {}
    virtual void doSemantic()
    {

    }
    std::shared_ptr<BlockAST> getWhileBlock()
    {
        return m_whileBlock;
    }
    llvm::Value* codegen() override;
private:
    std::shared_ptr<ASTNode> m_whileExpr;
    std::shared_ptr<BlockAST> m_whileBlock;
};