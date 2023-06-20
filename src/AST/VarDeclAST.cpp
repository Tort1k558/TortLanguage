#include"VarDeclAST.h"

#include"LLVMCallFuncAST.h"
#include"BlockAST.h"
void VarDeclAST::doSemantic()
{
    LLVMManager& manager = LLVMManager::getInstance();
    auto builder = manager.getBuilder();
    auto symbolTable = SymbolTableManager::getInstance().getSymbolTable();

    if (!m_sizeArrayAST.empty())
    {
        llvmType = builder->getPtrTy();
        m_containedType = getType(m_type);
        symbolTable->addNode(shared_from_this());
        return;
    }

    llvmType = getType(m_type);
    if (m_type == TokenType::Var)
    {
        if (!m_value)
        {
            throw std::runtime_error("ERROR::AST::The variable of type var must be initialized immediately");
        }
        m_value->doSemantic();
        llvmType = m_value->llvmType;
    }
    symbolTable->addNode(shared_from_this());
}

void VarDeclAST::codegen()
{
    LLVMManager& manager = LLVMManager::getInstance();
    auto context = manager.getContext();
    auto builder = manager.getBuilder();
    auto module = manager.getModule();
    auto symbolTable = SymbolTableManager::getInstance().getSymbolTable();

    if (!m_sizeArrayAST.empty())
    {
        std::vector<int> indexesVLA;
        std::vector<llvm::Value*> values;
        for (const auto& val : m_sizeArrayAST)
        {
            val->codegen();
            llvm::Value* value = val->getLValue();
            if (llvm::dyn_cast<llvm::ConstantInt>(value))
            {
                llvm::ConstantInt* constInt = llvm::dyn_cast<llvm::ConstantInt>(value);
                values.push_back(llvm::ConstantInt::get(builder->getInt64Ty(), constInt->getSExtValue()));
            }
            else
            {
                values.push_back(builder->CreateZExt(value, builder->getInt64Ty()));
            }
        }
        for (size_t i = 0; i < values.size(); i++)
        {
            if (!llvm::dyn_cast<llvm::ConstantInt>(values[i]))
            {
                indexesVLA.push_back(i);
            }
        }
        if (indexesVLA.empty())
        {
            //for simple arrays
            llvm::Type* containedType = getType(m_type);
            uint64_t sizeArray;
            for (size_t i = values.size() - 1; i > 0; i--)
            {
                sizeArray = llvm::dyn_cast<llvm::ConstantInt>(values[i])->getZExtValue();
                containedType = llvm::ArrayType::get(containedType, sizeArray);
            }
            sizeArray = llvm::dyn_cast<llvm::ConstantInt>(values[0])->getZExtValue();

            containedType = llvm::ArrayType::get(containedType, sizeArray);
            llvmType = containedType;

            llvm::AllocaInst* alloca = builder->CreateAlloca(llvmType, nullptr, m_name.c_str());
            symbolTable->addNode(shared_from_this());

            llvmValue = alloca;
            return;
        }
        else
        {
            //for VLA
            int lastIndexVLA = indexesVLA[indexesVLA.size() - 1];
            llvm::Value* sizeStack = builder->CreateZExt(values[0], builder->getInt64Ty());
            for (size_t i = 1; i <= lastIndexVLA; i++)
            {
                sizeStack = builder->CreateNUWMul(sizeStack, values[i], "multmp");
            }


            llvm::Function* stackSaveFunc = llvm::Intrinsic::getDeclaration(module.get(), llvm::Intrinsic::stacksave);
            llvm::Value* stackPtr = builder->CreateCall(stackSaveFunc);

            llvm::Type* containedType = getType(m_type);
            for (size_t i = values.size() - 1; i > lastIndexVLA; i--)
            {
                uint64_t sizeArray = llvm::dyn_cast<llvm::ConstantInt>(values[i])->getZExtValue();
                containedType = llvm::ArrayType::get(containedType, sizeArray);
            }
            llvm::AllocaInst* alloca = builder->CreateAlloca(containedType, sizeStack);
            m_sizeArrayVLA = std::vector<llvm::Value*>(values.begin(), values.begin() + lastIndexVLA + 1);
            symbolTable->addNode(shared_from_this());

            llvm::Function* stackRestoreFunc = llvm::Intrinsic::getDeclaration(module.get(), llvm::Intrinsic::stackrestore);

            std::shared_ptr<BlockAST> block = manager.getCurrentBlock();
            block->addStatementBeforeReturn(std::make_shared<LLVMCallFuncAST>(stackRestoreFunc, stackPtr));

            llvmValue = alloca;
            return;
        }
    }
    if (m_isReference)
    {
        if (!m_value)
        {
            throw std::runtime_error("ERROR::AST::The reference must be initialized when declaring!");
        }
        m_value->codegen();
        llvm::AllocaInst* alloca = builder->CreateAlloca(builder->getPtrTy(), nullptr, m_name.c_str());
        builder->CreateStore(m_value->getRValue(), alloca);
        symbolTable->addNode(shared_from_this());
        llvmValue = alloca;
        return;
    }
    llvm::AllocaInst* alloca = builder->CreateAlloca(llvmType, nullptr, m_name.c_str());
    llvm::Value* value = nullptr;
    if (!m_value)
    {
        if (llvmType->isDoubleTy())
        {
            value = llvm::ConstantFP::get(*context, llvm::APFloat(0.0));
        }
        else if (llvmType->isIntegerTy(1))
        {
            value = llvm::ConstantInt::get(llvm::Type::getInt1Ty(*context), false);
        }
        else if (llvmType->isIntegerTy())
        {
            value = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context), 0);
        }
        else if (llvmType->isPointerTy())
        {
            value = llvm::Constant::getNullValue(llvmType);;
        }
    }
    else
    {
        m_value->codegen();
        value = m_value->getLValue();
    }

    builder->CreateStore(value, alloca);
    symbolTable->addNode(shared_from_this());

    llvmValue = alloca;
}

void VarDeclAST::setValue(std::shared_ptr<ASTNode> value)
{
    m_value = value;
}

int VarDeclAST::getDimensionArray()
{
    return m_sizeArrayAST.size();
}

llvm::Type* VarDeclAST::getContainedType()
{
    return m_containedType;
}

bool VarDeclAST::isReference()
{
    return m_isReference;
}

std::vector<llvm::Value*> VarDeclAST::getSizeArrayVLA()
{
    return m_sizeArrayVLA;
}

llvm::Value* VarDeclAST::getRValue()
{
    LLVMManager& manager = LLVMManager::getInstance();
    auto builder = manager.getBuilder();
    if (!llvmValue)
    {
        throw std::runtime_error("The expression has no value!");
    }
    return llvmValue;
}

llvm::Value* VarDeclAST::getLValue()
{
    LLVMManager& manager = LLVMManager::getInstance();
    auto builder = manager.getBuilder();
    if (!llvmValue)
    {
        throw std::runtime_error("The expression has no value!");
    }
    llvm::AllocaInst* allocaInst = llvm::dyn_cast<llvm::AllocaInst>(llvmValue);
    if (allocaInst)
    {
        llvm::Type* varType = allocaInst->getAllocatedType();
        if (varType->isArrayTy())
        {
            llvm::Value* zero = llvm::ConstantInt::get(builder->getInt64Ty(), 0);
            return builder->CreateGEP(varType, llvmValue, { zero, zero }, "ptrtoelementarray", true);
        }
        return builder->CreateLoad(varType, allocaInst);
    }
    return llvmValue;
}
