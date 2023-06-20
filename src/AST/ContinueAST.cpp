#include"ContinueAST.h"

void ContinueAST::doSemantic()
{

}

void ContinueAST::codegen()
{
    LLVMManager& manager = LLVMManager::getInstance();
    auto builder = manager.getBuilder();
    codegenCompletionsStatements();
    builder->CreateBr(m_nextBlock);
}

void ContinueAST::setNextBlock(llvm::BasicBlock* block)
{
    m_nextBlock = block;
}