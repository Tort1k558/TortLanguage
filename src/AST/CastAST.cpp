#include"CastAST.h"

void CastAST::doSemantic()
{
    llvmType = getType(m_type);
}

void CastAST::codegen()
{
    LLVMManager& manager = LLVMManager::getInstance();
    auto builder = manager.getBuilder();

    m_value->codegen();
    llvm::Value* value = m_value->getLValue();
    llvm::Type* type = getType(m_type);

    if (type == value->getType())
    {
        llvmValue = value;
    }
    else if (type->isDoubleTy() && value->getType()->isIntegerTy())
    {
        llvmValue = builder->CreateSIToFP(value, type, "sitofptmp");
    }
    else if (type->isIntegerTy() && value->getType()->isDoubleTy())
    {
        llvmValue = builder->CreateFPToSI(value, type, "sitofptmp");
    }
    else if (type->isIntegerTy(1) && value->getType()->isIntegerTy())
    {
        llvmValue = builder->CreateICmpNE(value, builder->getInt32(0), "i32toi1tmp");
    }
    else if (type->isIntegerTy() && value->getType()->isIntegerTy(1))
    {
        llvmValue = builder->CreateIntCast(value, type, false, "i1toi32tmp");
    }
    else
    {
        throw std::runtime_error("ERROR::AST::Invalid Cast type!");
    }
}
