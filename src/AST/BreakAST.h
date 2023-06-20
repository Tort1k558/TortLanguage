#pragma once

#include"AST.h"
#include"CompletionInstruction.h"

class BreakAST : public ASTNode, public CompletionInstruction
{
public:
    BreakAST() :
        m_nextBlock(nullptr) {}
    void doSemantic() override;
    void codegen() override;
    void setNextBlock(llvm::BasicBlock* block);
private:
    llvm::BasicBlock* m_nextBlock;
};