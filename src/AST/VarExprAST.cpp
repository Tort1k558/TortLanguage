#include"VarExprAST.h"

void VarExprAST::doSemantic()
{
    llvmType = SymbolTableManager::getInstance().getSymbolTable()->getNode(m_name)->llvmType;
}

void VarExprAST::codegen()
{
    LLVMManager& manager = LLVMManager::getInstance();
    auto builder = manager.getBuilder();
    auto symbolTable = SymbolTableManager::getInstance().getSymbolTable();
    m_var = std::dynamic_pointer_cast<VarDeclAST>(symbolTable->getNode(m_name));
    if (m_var->isReference())
    {
        llvmValue = builder->CreateLoad(builder->getPtrTy(), m_var->getRValue());
    }
    else
    {
        llvmValue = m_var->getRValue();
    }
}

llvm::Value* VarExprAST::getRValue()
{
    return llvmValue;
}

llvm::Value* VarExprAST::getLValue()
{
    LLVMManager& manager = LLVMManager::getInstance();
    auto builder = manager.getBuilder();
    if (m_var->isReference())
    {
        return builder->CreateLoad(m_var->llvmType, llvmValue);
    }
    llvm::AllocaInst* allocaInst = llvm::dyn_cast<llvm::AllocaInst>(llvmValue);
    if (!allocaInst)
    {
        throw std::runtime_error("ERROR::AST::Variable has no address!");
    }
    llvm::Type* varType = allocaInst->getAllocatedType();
    if (varType->isArrayTy())
    {
        return llvmValue;
    }
    return builder->CreateLoad(varType, allocaInst);

}