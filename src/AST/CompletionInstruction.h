#pragma once

#include"AST.h"

class CompletionInstruction
{
public:
    void addStatementBeforeCompleting(std::shared_ptr<ASTNode> stmt);
    void codegenCompletionsStatements();
private:
    std::vector<std::shared_ptr<ASTNode>> m_statements;
};
