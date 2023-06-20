#include"LLVMValueAST.h"

void LLVMValueAST::doSemantic()
{
    llvmType = m_value->getType();
}

void LLVMValueAST::codegen()
{
    llvmValue = m_value;
}