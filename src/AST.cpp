#include "AST.h"

llvm::Type* getType(llvm::LLVMContext* context, TokenType type)
{
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
    default:
        break;
    }
    std::cout << "ERROR::UnknownType::" << g_nameTypes[static_cast<int>(type)];
}