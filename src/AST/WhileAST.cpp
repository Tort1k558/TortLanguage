#include"WhileAST.h"

#include"BlockAST.h"

void WhileAST::doSemantic()
{

}

void WhileAST::codegen()
{
    LLVMManager& manager = LLVMManager::getInstance();
    auto builder = manager.getBuilder();
    auto context = manager.getContext();

    llvm::Function* function = builder->GetInsertBlock()->getParent();

    llvm::BasicBlock* exprBB = llvm::BasicBlock::Create(*context, "whileexprblock");
    llvm::BasicBlock* whileBB = llvm::BasicBlock::Create(*context, "whileblock");
    llvm::BasicBlock* mergeBB = llvm::BasicBlock::Create(*context, "mergeblock");

    //generate exprblock
    function->insert(function->end(), exprBB);
    builder->CreateBr(exprBB);
    builder->SetInsertPoint(exprBB);
    m_whileExpr->codegen();
    llvm::Value* value = m_whileExpr->getLValue();
    builder->CreateCondBr(value, whileBB, mergeBB);

    //generate whileblock
    function->insert(function->end(), whileBB);
    builder->SetInsertPoint(whileBB);
    for (const auto& brk : m_whileBlock->getBreaks())
    {
        brk->setNextBlock(mergeBB);
    }
    for (const auto& cont : m_whileBlock->getContinuations())
    {
        cont->setNextBlock(exprBB);
    }
    m_whileBlock->codegen();
    builder->CreateBr(exprBB);

    //continue block
    function->insert(function->end(), mergeBB);
    builder->SetInsertPoint(mergeBB);
}

std::shared_ptr<BlockAST> WhileAST::getWhileBlock()
{
    return m_whileBlock;
}