#include"LiteralExprAST.h"

void LiteralExprAST::doSemantic()
{
    LLVMManager& manager = LLVMManager::getInstance();
    auto context = manager.getContext();
    auto builder = manager.getBuilder();

    if (m_type == TokenType::DoubleLiteral)
    {
        llvmType = builder->getDoubleTy();
    }
    else if (m_type == TokenType::IntLiteral)
    {
        llvmType = builder->getInt32Ty();
    }
    else if (m_type == TokenType::TrueLiteral || m_type == TokenType::FalseLiteral)
    {
        llvmType = builder->getInt1Ty();
    }
    else if (m_type == TokenType::StringLiteral)
    {
        llvmType = builder->getInt8PtrTy();
    }
}

void LiteralExprAST::codegen()
{
    LLVMManager& manager = LLVMManager::getInstance();
    auto context = manager.getContext();
    auto builder = manager.getBuilder();

    if (m_type == TokenType::DoubleLiteral)
    {
        llvmValue = llvm::ConstantFP::get(*context, llvm::APFloat(std::stod(m_value)));
    }
    else if (m_type == TokenType::IntLiteral)
    {
        llvmValue = builder->getInt32(std::stoi(m_value));
    }
    else if (m_type == TokenType::TrueLiteral)
    {
        llvmValue = builder->getInt1(true);
    }
    else if (m_type == TokenType::FalseLiteral)
    {
        llvmValue = builder->getInt1(false);
    }
    else if (m_type == TokenType::StringLiteral)
    {
        llvm::Constant* globalString = builder->CreateGlobalStringPtr(m_value, ".str");
        llvm::Value* zero = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context), 0);
        llvmValue = builder->CreateConstInBoundsGEP1_32(globalString->getType(), globalString, 0, "strptr");
    }
    else
    {
        llvmValue = nullptr;
    }
}

llvm::Value* LiteralExprAST::getRValue()
{
    throw std::runtime_error("The expression cannot return RValue");
}

llvm::Value* LiteralExprAST::getLValue()
{
    if (!llvmValue)
    {
        throw std::runtime_error("The expression has no value!");
    }
    return llvmValue;
}