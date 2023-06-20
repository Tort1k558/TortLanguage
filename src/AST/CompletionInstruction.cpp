#include"CompletionInstruction.h"

void CompletionInstruction::addStatementBeforeCompleting(std::shared_ptr<ASTNode> stmt)
{
    m_statements.push_back(stmt);
}
void CompletionInstruction::codegenCompletionsStatements()
{
    for (const auto& stmt : m_statements)
    {
        stmt->codegen();
    }
}
