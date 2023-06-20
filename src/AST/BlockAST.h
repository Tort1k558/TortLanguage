#pragma once

#include"AST.h"
#include"ReturnAST.h"
#include"BreakAST.h"
#include"ContinueAST.h"

class BlockAST : public ASTNode {
public:
    BlockAST(std::shared_ptr<SymbolTable> symbolTable)
        : m_symbolTable(std::make_shared<SymbolTable>())
    {
        m_symbolTable->extend(symbolTable.get());
    }
    void doSemantic() override;
    void codegen() override;

    void addStatement(std::shared_ptr<ASTNode> stmt);
    void addStatementBeforeReturn(std::shared_ptr<ASTNode> stmt);
    void extendSymbolTable(std::shared_ptr<SymbolTable> symbolTable);

    std::vector<std::shared_ptr<ReturnAST>> getReturns();
    std::vector<std::shared_ptr<BreakAST>> getBreaks();
    std::vector<std::shared_ptr<ContinueAST>> getContinuations();
private:
    std::vector<std::shared_ptr<ASTNode>> m_statements;
    std::shared_ptr<SymbolTable> m_symbolTable;
    std::vector<std::shared_ptr<ASTNode>> m_beforeRetStmts;
};