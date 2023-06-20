#include"GotoAST.h"

void GotoAST::doSemantic()
{

}

void GotoAST::codegen()
{
    LLVMManager& manager = LLVMManager::getInstance();
    auto builder = manager.getBuilder();
    if (m_gotoelseBB && m_value)
    {
        builder->CreateCondBr(m_value, m_gotoBB, m_gotoelseBB);
    }
    else
    {
        builder->CreateBr(m_gotoBB);
    }
    llvmValue = m_value;
}