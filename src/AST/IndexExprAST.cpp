#include"IndexExprAST.h"
#include"VarDeclAST.h"

void IndexExprAST::doSemantic()
{
    LLVMManager& manager = LLVMManager::getInstance();
    auto builder = manager.getBuilder();
    auto symbolTable = SymbolTableManager::getInstance().getSymbolTable();
    std::shared_ptr<VarDeclAST> arr = std::dynamic_pointer_cast<VarDeclAST>(symbolTable->getNode(m_name));
    if (m_indexes.size() < arr->getDimensionArray())
    {
        llvmType = builder->getPtrTy();
    }
    else
    {
        llvmType = arr->getContainedType();
    }
}

void IndexExprAST::codegen()
{
    LLVMManager& manager = LLVMManager::getInstance();
    auto builder = manager.getBuilder();
    auto context = manager.getContext();
    auto symbolTable = SymbolTableManager::getInstance().getSymbolTable();
    std::vector<llvm::Value*> valIndexes;
    for (const auto& index : m_indexes)
    {
        index->codegen();
        valIndexes.push_back(builder->CreateZExt(index->getLValue(), builder->getInt64Ty()));
    }
    std::shared_ptr<VarDeclAST> var = std::dynamic_pointer_cast<VarDeclAST>(symbolTable->getNode(m_name));
    llvm::Value* varPtr = var->getRValue();
    llvm::Type* varType = var->llvmType;
    std::vector<llvm::Value*> sizesVLA = var->getSizeArrayVLA();

    if (!sizesVLA.empty())
    {
        for (size_t i = 0; (i < m_indexes.size()) && (i < sizesVLA.size()); i++)
        {
            llvm::Value* index = nullptr;
            if (i == sizesVLA.size() - 1)
            {
                index = valIndexes[i];
            }
            else
            {
                index = sizesVLA[sizesVLA.size() - 1];
                if (i == sizesVLA.size() - 2)
                {
                    index = builder->CreateNSWMul(index, valIndexes[i], "multmp");
                }
                else
                {
                    for (size_t j = i + 1; j < sizesVLA.size(); j++)
                    {
                        if (llvm::dyn_cast<llvm::ConstantInt>(sizesVLA[j]))
                        {
                            llvm::Value* constInt = sizesVLA[j];
                            j++;
                            while (llvm::dyn_cast<llvm::ConstantInt>(sizesVLA[j]))
                            {
                                constInt = builder->CreateNUWMul(constInt, sizesVLA[j], "multmp");
                                j++;
                            }
                            index = builder->CreateNUWMul(index, constInt, "multmp");
                        }
                        else
                        {
                            index = builder->CreateNUWMul(index, sizesVLA[j], "multmp");
                        }
                    }
                    index = builder->CreateNSWMul(index, valIndexes[i], "multmp");
                }
            }
            varPtr = builder->CreateGEP(varType, varPtr, index, "ptrtoelementarray", true);
        }
    }
    llvm::Value* zero = llvm::ConstantInt::get(builder->getInt64Ty(), 0);
    int lastIndexVLA = 0;
    if (!sizesVLA.empty())
    {
        lastIndexVLA = sizesVLA.size();
    }
    for (size_t i = lastIndexVLA; i < valIndexes.size(); i++)
    {
        if (llvm::dyn_cast<llvm::ArrayType>(varType))
        {
            varPtr = builder->CreateGEP(varType, varPtr, { zero, valIndexes[i] }, "ptrtoelementarray", true);
            llvm::GetElementPtrInst* val = llvm::dyn_cast<llvm::GetElementPtrInst>(varPtr);
            varType = val->getResultElementType();
        }
    }
    llvmValue = varPtr;
}

llvm::Value* IndexExprAST::getRValue()
{
    if (!llvmValue)
    {
        throw std::runtime_error("The expression has no value!");
    }
    return llvmValue;
}

llvm::Value* IndexExprAST::getLValue()
{
    LLVMManager& manager = LLVMManager::getInstance();
    auto builder = manager.getBuilder();
    auto symbolTable = SymbolTableManager::getInstance().getSymbolTable();
    if (!llvmValue)
    {
        throw std::runtime_error("The expression has no value!");
    }
    std::shared_ptr<VarDeclAST> arr = std::dynamic_pointer_cast<VarDeclAST>(symbolTable->getNode(m_name));
    llvm::GetElementPtrInst* GEPInst = llvm::dyn_cast<llvm::GetElementPtrInst>(llvmValue);
    if (GEPInst)
    {
        llvm::Type* varType = GEPInst->getResultElementType();
        llvm::Value* zero = llvm::ConstantInt::get(builder->getInt64Ty(), 0);
        if (varType->isArrayTy())
        {
            return builder->CreateGEP(varType, llvmValue, { zero, zero }, "ptrtoelementarray", true);
        }
        if (arr->getDimensionArray() > m_indexes.size())
        {
            return llvmValue;
        }
        return builder->CreateLoad(varType, llvmValue);
    }
    return llvmValue;
}