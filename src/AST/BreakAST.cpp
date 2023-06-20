#include"BreakAST.h"

void BreakAST::doSemantic()
{

}

void BreakAST::codegen()
{
    LLVMManager& manager = LLVMManager::getInstance();
    auto builder = manager.getBuilder();
    codegenCompletionsStatements();
    builder->CreateBr(m_nextBlock);
}

void BreakAST::setNextBlock(llvm::BasicBlock* block)
{
    m_nextBlock = block;
}