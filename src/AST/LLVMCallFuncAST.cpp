#include"LLVMCallFuncAST.h"

void LLVMCallFuncAST::doSemantic()
{
    llvmType = m_function->getFunctionType()->getReturnType();
}

void LLVMCallFuncAST::codegen()
{
    LLVMManager& manager = LLVMManager::getInstance();
    auto builder = manager.getBuilder();
    llvm::Value* val = m_args[0];
    llvmValue = builder->CreateCall(m_function, m_args);
}