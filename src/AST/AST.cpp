#include "AST.h"
#include"../LLVMManager.h"

llvm::Type* getType(TokenType type)
{
    LLVMManager& manager = LLVMManager::getInstance();
    auto context = manager.getContext();

    switch (type)
    {
    case TokenType::Bool:
        return llvm::Type::getInt1Ty(*context);
    case TokenType::Int:
        return llvm::Type::getInt32Ty(*context);
    case TokenType::Double:
        return llvm::Type::getDoubleTy(*context);
    case TokenType::Void:
        return llvm::Type::getVoidTy(*context);
    case TokenType::String:
        return llvm::Type::getInt8PtrTy(*context);
    case TokenType::Var:
    case TokenType::NonType:
        return nullptr;
    default:
        break;
    }
    throw std::runtime_error("ERROR::UnknownType " + g_nameTypes[static_cast<int>(type)]);
}

llvm::Value* castType(llvm::Value* value, llvm::Type* type)
{
    LLVMManager& manager = LLVMManager::getInstance();
    auto builder = manager.getBuilder();

    if (type == value->getType())
    {
        return value;
    }
    else if (type->isDoubleTy() && value->getType()->isIntegerTy())
    {
        return builder->CreateSIToFP(value, type, "sitofptmp");
    }
    else if (type->isIntegerTy() && value->getType()->isDoubleTy())
    {
        return builder->CreateFPToSI(value, type, "sitofptmp");
    }
    else if (type->isIntegerTy(1) && value->getType()->isIntegerTy())
    {
        return builder->CreateICmpNE(value, builder->getInt32(0), "i32toi1tmp");

    }
    else
    {
        throw std::runtime_error("ERROR::AST::Invalid Cast type!");
    }
}