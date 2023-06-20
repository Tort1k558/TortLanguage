#include"ReturnAST.h"

void ReturnAST::doSemantic()
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

void ReturnAST::codegen()
{
    LLVMManager& manager = LLVMManager::getInstance();
    auto builder = manager.getBuilder();
    auto context = manager.getContext();

    if (m_returnBB)
    {
        if (m_returnExpr)
        {
            m_returnExpr->codegen();
            llvm::Value* returnValue = m_returnExpr->getLValue();
            builder->CreateStore(returnValue, m_returnVar);
            codegenCompletionsStatements();
            builder->CreateBr(m_returnBB);
        }
        else
        {
            codegenCompletionsStatements();
            builder->CreateBr(m_returnBB);
        }
    }
    else
    {
        if (m_returnExpr)
        {
            m_returnExpr->codegen();
            llvm::Value* retVal = m_returnExpr->getLValue();
            codegenCompletionsStatements();
            builder->CreateRet(retVal);
        }
        else
        {
            codegenCompletionsStatements();
            builder->CreateRetVoid();
        }
    }
}

void ReturnAST::setReturnBB(llvm::BasicBlock* BB)
{
    m_returnBB = BB;
}

void ReturnAST::setReturnVar(llvm::AllocaInst* var)
{
    m_returnVar = var;
}

std::shared_ptr<ASTNode> ReturnAST::getReturnExpr()
{
    return m_returnExpr;
}
