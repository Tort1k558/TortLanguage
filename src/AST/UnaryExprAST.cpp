#include"UnaryExprAST.h"
#include"VarExprAST.h"

void UnaryExprAST::doSemantic()
{
    auto symbolTable = SymbolTableManager::getInstance().getSymbolTable();
    llvmType = symbolTable->getNode(m_name)->llvmType;
}

void UnaryExprAST::codegen()
{
    LLVMManager& manager = LLVMManager::getInstance();
    auto builder = manager.getBuilder();
    auto context = manager.getContext();
    auto symbolTable = SymbolTableManager::getInstance().getSymbolTable();
    std::shared_ptr<VarExprAST> var = std::make_shared<VarExprAST>(m_name);
    var->codegen();
    llvm::Value* varLocation = var->getRValue();
    llvm::Value* varValue = var->getLValue();
    llvm::Type* varType = varValue->getType();
    switch (m_op)
    {
    case TokenType::Increment:
    {
        llvm::Value* incrementResult = nullptr;
        if (varType->isDoubleTy())
        {
            incrementResult = builder->CreateFAdd(varValue, llvm::ConstantFP::get(*context, llvm::APFloat(1.0)), "incrementtmp");
        }
        else if (varType->isIntegerTy(1))
        {
            incrementResult = builder->CreateAdd(varValue, builder->getInt1(1), "incrementtmp");
        }
        else if (varType->isIntegerTy())
        {
            incrementResult = builder->CreateAdd(varValue, builder->getInt32(1), "incrementtmp");
        }
        builder->CreateStore(incrementResult, varLocation);

        if (m_prefix)
        {
            llvmValue = incrementResult;
        }
        else
        {
            llvmValue = varValue;
        }
        break;
    }

    case TokenType::Decrement:
    {
        llvm::Value* decrementResult = nullptr;
        if (varType->isDoubleTy())
        {
            decrementResult = builder->CreateFSub(varValue, llvm::ConstantFP::get(*context, llvm::APFloat(1.0)), "decrementtmp");
        }
        else if (varType->isIntegerTy())
        {
            decrementResult = builder->CreateSub(varValue, builder->getInt32(1), "decrementtmp");
        }
        builder->CreateStore(decrementResult, varLocation);

        if (m_prefix)
        {
            llvmValue = decrementResult;
        }
        else
        {
            llvmValue = varValue;
        }
        break;
    }
    }
}

