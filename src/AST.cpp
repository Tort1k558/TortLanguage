#include "AST.h"
#include"LLVMManager.h"

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
void LLVMValueAST::codegen()
{
    llvmValue = m_value;
}
void LLVMCallFuncAST::codegen()
{
    LLVMManager& manager = LLVMManager::getInstance();
    auto builder = manager.getBuilder();
    llvm::Value* val = m_args[0];
    llvmValue = builder->CreateCall(m_function, m_args);
}
void VarDeclAST::codegen()
{
    LLVMManager& manager = LLVMManager::getInstance();
    auto context = manager.getContext();
    auto builder = manager.getBuilder();
    auto module = manager.getModule();
    auto symbolTable  = SymbolTableManager::getInstance().getSymbolTable();

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
                values.push_back(llvm::ConstantInt::get(builder->getInt64Ty(),constInt->getSExtValue()));
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
            for (size_t i = values.size()-1; i > 0; i--)
            {
                sizeArray = llvm::dyn_cast<llvm::ConstantInt>(values[i])->getZExtValue();
                containedType = llvm::ArrayType::get(containedType, sizeArray);
            }
            sizeArray = llvm::dyn_cast<llvm::ConstantInt>(values[0])->getZExtValue();

            containedType = llvm::ArrayType::get(containedType, sizeArray);
            llvmType = containedType;
            
            llvm::AllocaInst* alloca = builder->CreateAlloca(llvmType, nullptr, m_name.c_str());
            symbolTable->addVarArray(m_name, alloca, containedType,values.size());

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
            for (size_t i = values.size()-1; i > lastIndexVLA; i--)
            {
                uint64_t sizeArray = llvm::dyn_cast<llvm::ConstantInt>(values[i])->getZExtValue();
                containedType = llvm::ArrayType::get(containedType, sizeArray);
            }
            llvm::AllocaInst* alloca = builder->CreateAlloca(containedType, sizeStack);
            symbolTable->addVarArray(m_name, alloca, containedType, values.size(), std::vector<llvm::Value*>(values.begin(), values.begin() + lastIndexVLA + 1));

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
        llvmValue = m_value->getRValue();
        symbolTable->addVar(m_name, llvmValue);
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
    symbolTable->addVar(m_name, alloca);

    llvmValue = alloca;
}

void VarExprAST::codegen()
{
    LLVMManager& manager = LLVMManager::getInstance();
    auto builder = manager.getBuilder();
    auto symbolTable = SymbolTableManager::getInstance().getSymbolTable();
    llvm::Value* var = nullptr;
    llvmValue = symbolTable->getPtrVar(m_name);
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

void BinaryExprAST::codegen()
{
    LLVMManager& manager = LLVMManager::getInstance();
    auto builder = manager.getBuilder();
    auto module = manager.getModule();
    auto context = manager.getContext();
    
    //for these operators, the code must be generated differently
    switch (m_op)
    {
    case TokenType::Assign:
    {
        m_lhs->codegen();
        llvm::Value* var = m_lhs->getRValue();
        m_rhs->codegen();
        llvm::Value* value = m_rhs->getLValue();
        if (!value) {
            llvmValue = nullptr;
        }
        builder->CreateStore(value, var);
        llvmValue = value;
        return;
    }
    case TokenType::Or:
    {
        m_lhs->codegen();
        llvm::Value* lhsValue = m_lhs->getLValue();
        lhsValue = castType(lhsValue, builder->getInt1Ty());

        llvm::Function* function = builder->GetInsertBlock()->getParent();

        llvm::BasicBlock* previousBB = builder->GetInsertBlock();
        llvm::BasicBlock* rhsBB = llvm::BasicBlock::Create(*context, "rhsblock");
        llvm::BasicBlock* mergeBB = llvm::BasicBlock::Create(*context, "mergeblock");


        builder->CreateCondBr(castType(lhsValue, builder->getInt1Ty()), mergeBB, rhsBB);

        function->insert(function->end(), rhsBB);
        builder->SetInsertPoint(rhsBB);
        m_rhs->codegen();
        llvm::Value* rhsValue = m_rhs->getLValue();
        rhsValue = castType(rhsValue, builder->getInt1Ty());
        builder->CreateBr(mergeBB);
        rhsBB = builder->GetInsertBlock();

        function->insert(function->end(), mergeBB);
        builder->SetInsertPoint(mergeBB);

        llvm::PHINode* phiNode = builder->CreatePHI(builder->getInt1Ty(), 2, "orresult");
        phiNode->addIncoming(lhsValue, previousBB);
        phiNode->addIncoming(rhsValue, rhsBB);

        llvmValue = phiNode;
        return;
    }
    case TokenType::And:
    {
        m_lhs->codegen();
        llvm::Value* lhsValue = m_lhs->getLValue();
        lhsValue = castType(lhsValue, builder->getInt1Ty());

        llvm::Function* function = builder->GetInsertBlock()->getParent();

        llvm::BasicBlock* previousBB = builder->GetInsertBlock();
        llvm::BasicBlock* rhsBB = llvm::BasicBlock::Create(*context, "rhsblock");
        llvm::BasicBlock* mergeBB = llvm::BasicBlock::Create(*context, "mergeblock");


        builder->CreateCondBr(castType(lhsValue, builder->getInt1Ty()), rhsBB, mergeBB);

        function->insert(function->end(), rhsBB);
        builder->SetInsertPoint(rhsBB);

        m_rhs->codegen();
        llvm::Value* rhsValue = m_rhs->getLValue();
        rhsValue = castType(rhsValue, builder->getInt1Ty());
        builder->CreateBr(mergeBB);
        rhsBB = builder->GetInsertBlock();

        function->insert(function->end(), mergeBB);
        builder->SetInsertPoint(mergeBB);

        llvm::PHINode* phiNode = builder->CreatePHI(builder->getInt1Ty(), 2, "orresult");
        phiNode->addIncoming(lhsValue, previousBB);
        phiNode->addIncoming(rhsValue, rhsBB);

        llvmValue = phiNode;
        return;
    }
    }

    m_lhs->codegen();
    llvm::Value* lhsValue = m_lhs->getLValue();
    m_rhs->codegen();
    llvm::Value* rhsValue = m_rhs->getLValue();

    if (!lhsValue || !rhsValue) {
        llvmValue = nullptr;
    }
    llvm::Type* lhsType = lhsValue->getType();
    llvm::Type* rhsType = rhsValue->getType();
    if (lhsType != rhsType)
    {
        rhsValue = castType(rhsValue,lhsType);
        rhsType = lhsType;
    }
    switch (m_op) {
    case TokenType::Plus:
        if (lhsType->isDoubleTy())
        {
            llvmValue = builder->CreateFAdd(lhsValue, rhsValue, "addtmp");
        }
        else if (lhsType->isIntegerTy())
        {
            llvmValue = builder->CreateAdd(lhsValue, rhsValue, "addtmp");
        }
        else
        {
            throw std::runtime_error("ERROR::AST::Invalid type for binary operation " + lhsType->getStructName().str());
        }
        return;
    case TokenType::Minus:
        if (lhsType->isDoubleTy())
        {
            llvmValue = builder->CreateFSub(lhsValue, rhsValue, "subtmp");
        }
        else if (lhsType->isIntegerTy())
        {
            llvmValue = builder->CreateSub(lhsValue, rhsValue, "subtmp");
        }
        else
        {
            throw std::runtime_error("ERROR::AST::Invalid type for binary operation " + lhsType->getStructName().str());
        }
        return;
    case TokenType::Mul:
        if (lhsType->isDoubleTy())
        {
            llvmValue = builder->CreateFMul(lhsValue, rhsValue, "multmp");
        }
        else if (lhsType->isIntegerTy())
        {
            llvmValue = builder->CreateMul(lhsValue, rhsValue, "multmp");
        }
        else
        {
            throw std::runtime_error("ERROR::AST::Invalid type for binary operation " + lhsType->getStructName().str());
        }
        return;
    case TokenType::Div:
        if (lhsType->isDoubleTy())
        {
            llvmValue = builder->CreateFDiv(lhsValue, rhsValue, "divtmp");
        }
        else if (lhsType->isIntegerTy())
        {
            llvmValue = builder->CreateSDiv(lhsValue, rhsValue, "divtmp");
        }
        else
        {
            throw std::runtime_error("ERROR::AST::Invalid type for binary operation " + lhsType->getStructName().str());
        }
        return;
    case TokenType::Less:
        if (lhsType->isDoubleTy())
        {
            llvmValue = builder->CreateFCmpULT(lhsValue, rhsValue, "lesstmp");
        }
        else if (lhsType->isIntegerTy())
        {
            llvmValue = builder->CreateICmpSLT(lhsValue, rhsValue, "lesstmp");
        }
        else
        {
            throw std::runtime_error("ERROR::AST::Invalid type for binary operation " + lhsType->getStructName().str());
        }
        return;
    case TokenType::Greater:
        if (lhsType->isDoubleTy())
        {
            llvmValue = builder->CreateFCmpUGT(lhsValue, rhsValue, "greatertmp");
        }
        else if (lhsType->isIntegerTy())
        {
            llvmValue = builder->CreateICmpSGT(lhsValue, rhsValue, "greatertmp");
        }
        else
        {
            throw std::runtime_error("ERROR::AST::Invalid type for binary operation " + lhsType->getStructName().str());
        }
        return;
    case TokenType::Equal:
        if (lhsType->isDoubleTy())
        {
            llvmValue = builder->CreateFCmpUEQ(lhsValue, rhsValue, "equaltmp");
        }
        else if (lhsType->isIntegerTy())
        {
            llvmValue = builder->CreateICmpEQ(lhsValue, rhsValue, "equaltmp");
        }
        else
        {
            throw std::runtime_error("ERROR::AST::Invalid type for binary operation " + lhsType->getStructName().str());
        }
        return;
    case TokenType::BitAnd:
        llvmValue = builder->CreateAnd(lhsValue, rhsValue, "bitandtmp");
        return;
    case TokenType::BitOr:
        llvmValue = builder->CreateOr(lhsValue, rhsValue, "bitortmp");
        return;
    case TokenType::Exponentiation:
    {
        if (lhsType->isIntegerTy())
        {
            lhsValue = castType(lhsValue, builder->getDoubleTy());
        }
        if (rhsType->isIntegerTy())
        {
            rhsValue = castType(rhsValue, builder->getDoubleTy());
        }

        llvm::Function* powFunction = llvm::Intrinsic::getDeclaration(module.get(), llvm::Intrinsic::pow, {lhsValue->getType()});
        llvm::Value* result = builder->CreateCall(powFunction, { lhsValue,rhsValue }, "powtmp");

        if (lhsType->isIntegerTy())
        {
            llvmValue = castType(result, lhsType);
        }

        llvmValue = result;
        return;
    }
    default:
        throw std::runtime_error("ERROR::AST::Invalid binary operator: " + g_nameTypes[static_cast<int>(m_op)]);
    }
}

void UnaryExprAST::codegen()
{
    LLVMManager& manager = LLVMManager::getInstance();
    auto builder = manager.getBuilder();
    auto context = manager.getContext();
    auto symbolTable = SymbolTableManager::getInstance().getSymbolTable();
    llvm::Value* varLocation = symbolTable->getPtrVar(m_name);
    llvm::Value* varValue = symbolTable->getValueVar(m_name);
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
    llvm::Value* varPtr = symbolTable->getValueVar(m_name);
    llvm::Type* varType = symbolTable->getTypeVar(m_name);
    std::vector<llvm::Value*> sizesVLA = symbolTable->getSizeArrayVLA(m_name);
    
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
                index = sizesVLA[sizesVLA.size()-1];
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
    for (size_t i = lastIndexVLA; i < valIndexes.size();i++)
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

void ConsoleOutputExprAST::codegen()
{
    LLVMManager& manager = LLVMManager::getInstance();
    auto context = manager.getContext();
    auto module = manager.getModule();
    auto builder = manager.getBuilder();
    auto symbolTable = SymbolTableManager::getInstance().getSymbolTable();

    llvm::Function* printFunc = module->getFunction("printf");
    if (!printFunc)
    {
        llvm::FunctionType* printFuncType = llvm::FunctionType::get(llvm::IntegerType::getInt32Ty(*context), llvm::PointerType::get(llvm::IntegerType::getInt8Ty(*context), 0), true);
        printFunc = llvm::Function::Create(printFuncType, llvm::Function::ExternalLinkage, "printf", *module);
    }
    if (!printFunc) {
        throw std::runtime_error("Printf function not found");
    }
    
    llvm::Value* value = nullptr;
    if (m_expr)

    {
        m_expr->codegen();
        value = m_expr->getLValue();
    }

    std::string formatStr = "%s\n";
    std::vector<llvm::Value*> printfArgs;
    if (value)
    {
        llvm::Type* i = value->getType();
        if (i->isFloatTy() || i->isDoubleTy()) {
            formatStr = "%f\n";
        }
        else if (i->isIntegerTy(32))
        {
            formatStr = "%d\n";
        }
        else if (i->isIntegerTy(1))
        {
            formatStr = "%s\n";
            llvm::Value* bool_str = builder->CreateGlobalStringPtr("%s");
            llvm::Value* true_str = builder->CreateGlobalStringPtr("true");
            llvm::Value* false_str = builder->CreateGlobalStringPtr("false");
            llvm::Value* result = builder->CreateSelect(value, true_str, false_str);
            printfArgs.push_back(bool_str);
            printfArgs.push_back(result);
            llvmValue = builder->CreateCall(printFunc, printfArgs);
        }
    }
    else
    {
        formatStr = "%s\n";
        printfArgs.push_back(builder->CreateGlobalStringPtr(formatStr));
        printfArgs.push_back(builder->CreateGlobalStringPtr(""));
        llvmValue = builder->CreateCall(printFunc, printfArgs);
        return;
    }

    printfArgs.push_back(builder->CreateGlobalStringPtr(formatStr));
    printfArgs.push_back(value);


    llvmValue = builder->CreateCall(printFunc, llvm::ArrayRef<llvm::Value* >(printfArgs));
}

void BlockAST::codegen() 
{
    LLVMManager& manager = LLVMManager::getInstance();
    auto builder = manager.getBuilder();

    for (size_t i = 0; i < m_statements.size(); i++) {
        if (std::dynamic_pointer_cast<BlockAST>(m_statements[i]))
        {
            manager.setCurrentBlock(std::dynamic_pointer_cast<BlockAST>(m_statements[i]));
        }
        m_statements[i]->codegen();

        //No need to generate code after return,break,continue
        if (std::dynamic_pointer_cast<ReturnAST>(m_statements[i]) || std::dynamic_pointer_cast<BreakAST>(m_statements[i]) || std::dynamic_pointer_cast<ContinueAST>(m_statements[i]))
        {
            break;
        }
    }
    llvm::BasicBlock* lastBB = builder->GetInsertBlock();
    if (lastBB->empty())
    {
        lastBB->removeFromParent();
    }
}

std::vector<std::shared_ptr<ReturnAST>> BlockAST::getReturns()
{
    std::vector<std::shared_ptr<ReturnAST>> returns;
    for (const auto& stmt : m_statements)
    {
        std::shared_ptr<ReturnAST> returnAST = std::dynamic_pointer_cast<ReturnAST>(stmt);
        if (returnAST)
        {
            returns.push_back(returnAST);
        }
        std::shared_ptr<BlockAST> blockAST = std::dynamic_pointer_cast<BlockAST>(stmt);
        if (blockAST)
        {
            for (const auto& ret : blockAST->getReturns())
            {
                returns.push_back(ret);
            }
        }

        std::shared_ptr<IfAST> ifAST= std::dynamic_pointer_cast<IfAST>(stmt);
        if (ifAST)
        {
            auto ifBlockAST = ifAST->getIfBlock();
            auto elseIfsBlockAST = ifAST->getElseIfs();
            auto elseBlockAST = ifAST->getElseBlock();
            for (const auto& ret : ifBlockAST->getReturns())
            {
                returns.push_back(ret);
            }
            if (!elseIfsBlockAST.empty())
            {
                for (size_t i = 0; i < elseIfsBlockAST.size(); i++)
                {
                    for (const auto& ret : elseIfsBlockAST[i].second->getReturns())
                    {
                        returns.push_back(ret);
                    }
                }
            }
            if (elseBlockAST)
            {
                for (const auto& ret : elseBlockAST->getReturns())
                {
                    returns.push_back(ret);
                }
            }
        }

        std::shared_ptr<WhileAST> whileAST = std::dynamic_pointer_cast<WhileAST>(stmt);
        if (whileAST)
        {
            for (const auto& ret : whileAST->getWhileBlock()->getReturns())
            {
                returns.push_back(ret);
            }
        }
    }
    return returns;
}
std::vector<std::shared_ptr<BreakAST>> BlockAST::getBreaks()
{
    std::vector<std::shared_ptr<BreakAST>> breaks;
    for (const auto& stmt : m_statements)
    {
        std::shared_ptr<BreakAST> breakAST = std::dynamic_pointer_cast<BreakAST>(stmt);
        if (breakAST)
        {
            breaks.push_back(breakAST);
        }
        std::shared_ptr<BlockAST> blockAST = std::dynamic_pointer_cast<BlockAST>(stmt);
        if (blockAST)
        {
            for (const auto& brk : blockAST->getBreaks())
            {
                breaks.push_back(brk);
            }
        }

        std::shared_ptr<IfAST> ifAST = std::dynamic_pointer_cast<IfAST>(stmt);
        if (ifAST)
        {
            auto ifBlockAST = ifAST->getIfBlock();
            auto elseIfsBlockAST = ifAST->getElseIfs();
            auto elseBlockAST = ifAST->getElseBlock();
            for (const auto& brk : ifBlockAST->getBreaks())
            {
                breaks.push_back(brk);
            }
            if (!elseIfsBlockAST.empty())
            {
                for (size_t i = 0; i < elseIfsBlockAST.size(); i++)
                {
                    for (const auto& brk : elseIfsBlockAST[i].second->getBreaks())
                    {
                        breaks.push_back(brk);
                    }
                }
            }
            if (elseBlockAST)
            {
                for (const auto& brk : elseBlockAST->getBreaks())
                {
                    breaks.push_back(brk);
                }
            }
        }
    }
    return breaks;
}
std::vector<std::shared_ptr<ContinueAST>> BlockAST::getContinuations()
{
    std::vector<std::shared_ptr<ContinueAST>> continuations;
    for (const auto& stmt : m_statements)
    {
        std::shared_ptr<ContinueAST> continueAST = std::dynamic_pointer_cast<ContinueAST>(stmt);
        if (continueAST)
        {
            continuations.push_back(continueAST);
        }
        std::shared_ptr<BlockAST> blockAST = std::dynamic_pointer_cast<BlockAST>(stmt);
        if (blockAST)
        {
            for (const auto& cont : blockAST->getContinuations())
            {
                continuations.push_back(cont);
            }
        }

        std::shared_ptr<IfAST> ifAST = std::dynamic_pointer_cast<IfAST>(stmt);
        if (ifAST)
        {
            auto ifBlockAST = ifAST->getIfBlock();
            auto elseIfsBlockAST = ifAST->getElseIfs();
            auto elseBlockAST = ifAST->getElseBlock();
            for (const auto& cont : ifBlockAST->getContinuations())
            {
                continuations.push_back(cont);
            }
            if (!elseIfsBlockAST.empty())
            {
                for (size_t i = 0; i < elseIfsBlockAST.size(); i++)
                {
                    for (const auto& cont : elseIfsBlockAST[i].second->getContinuations())
                    {
                        continuations.push_back(cont);
                    }
                }
            }
            if (elseBlockAST)
            {
                for (const auto& cont : elseBlockAST->getContinuations())
                {
                    continuations.push_back(cont);
                }
            }
        }
    }
    return continuations;
}

void ProtFunctionAST::codegen() 
{
    LLVMManager& manager = LLVMManager::getInstance();
    auto module = manager.getModule();
    std::vector<llvm::Type*> argTypes;
    for (const auto& arg : m_args) {
        argTypes.push_back(arg->llvmType);
    }
    if (!m_returnType)
    {
        throw std::runtime_error("ERROR::AST::The function declaration cannot be specified without the return value type");
    }
    llvm::FunctionType* funcType = llvm::FunctionType::get(m_returnType, argTypes, false);
    llvm::Function* func = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, m_name, module.get());
    unsigned int i = 0;
    for (auto& arg : func->args())
    {
        arg.setName("arg" + std::to_string(i));
    }

    m_prevSymbolTable->addFunction(m_name, func, m_args);
    llvmValue = func;
}

void FunctionAST::codegen()
{
    LLVMManager& manager = LLVMManager::getInstance();
    auto context = manager.getContext();
    auto module = manager.getModule();
    auto builder = manager.getBuilder();
    auto symbolTableFunc = SymbolTableManager::getInstance().getSymbolTable();

    //parseArgs
    std::vector<llvm::Type*> argTypes;
    for (const auto& arg : m_args) {
        arg->doSemantic();
        argTypes.push_back(arg->llvmType);
    }

    //createFunction
    llvm::Function* func =nullptr;
    std::vector<std::shared_ptr<ASTNode>> args;
    for (const auto& arg :m_args)
    {
        args.push_back(arg);
    }
    func = m_prevSymbolTable->getFunction(m_name, args);
    if (!func)
    {
        llvm::FunctionType* funcType = llvm::FunctionType::get(m_returnType, argTypes, false);
        func = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, m_name, module.get());
        m_prevSymbolTable->addFunction(m_name, func, m_args);
        symbolTableFunc->addFunction(m_name, func, m_args);
    }

    llvm::BasicBlock* entryBB = llvm::BasicBlock::Create(*context, "entry", func);
    builder->SetInsertPoint(entryBB);
    
    llvm::BasicBlock* returnBB = nullptr;
    if (m_returns.size() > 1)
    {
        returnBB = llvm::BasicBlock::Create(*context, "returnblock");
        for (const auto& ret : m_returns)
        {
            ret->setReturnBB(returnBB);
        }
    }

    llvm::AllocaInst* returnVar = nullptr;
    if (returnBB && !m_returnType->isVoidTy())
    {
        returnVar = builder->CreateAlloca(m_returnType, nullptr, "retvar");
        for (const auto& ret : m_returns)
        {
            ret->setReturnVar(returnVar);
        }
    }

    size_t i = 0;
    for (auto& arg : func->args())
    {
        arg.setName("arg"+std::to_string(i));
        m_args[i]->setValue(std::make_shared<LLVMValueAST>(&arg));
        m_args[i]->codegen();
        ++i;
    }

    m_body->extendSymbolTable(symbolTableFunc);
    manager.setCurrentBlock(m_body);
    m_body->codegen();
    if (returnBB)
    {
        returnBB->insertInto(func);
        builder->SetInsertPoint(returnBB);
        if (!m_returnType->isVoidTy())
        {
            builder->CreateRet(builder->CreateLoad(returnVar->getAllocatedType(), returnVar));
        }
        else
        {
            builder->CreateRetVoid();
        }
    }
    if (builder->GetInsertBlock()->getTerminator() == nullptr)
    {
        if (m_returnType->isVoidTy())
        {
            builder->CreateRetVoid();
        }
        else
        {
            builder->CreateRet(llvm::Constant::getNullValue(m_returnType));
        }
    }
    llvmValue = func;
}

void CallExprAST::codegen()
{
    LLVMManager& manager = LLVMManager::getInstance();
    auto module = manager.getModule();
    auto builder = manager.getBuilder();

    auto symbolTable = SymbolTableManager::getInstance().getSymbolTable();

    llvm::Function* func = symbolTable->getFunction(m_name,m_args);

    std::vector<llvm::Value*> args;
    for (const auto& arg : m_args) {
        arg->codegen();
        args.push_back(arg->getLValue());
    }

    llvmValue = builder->CreateCall(func, args);
}

void ReturnAST::codegen()
{
    LLVMManager& manager = LLVMManager::getInstance();
    auto builder = manager.getBuilder();
    auto context = manager.getContext();

    if (m_returnBB)
    {
        if (m_returnExpr)
        {
            m_returnExpr->codegen();
            llvm::Value* returnValue = m_returnExpr->getLValue();
            builder->CreateStore(returnValue, m_returnVar);
            codegenCompletionsStatements();
            builder->CreateBr(m_returnBB);
        }
        else
        {
            codegenCompletionsStatements();
            builder->CreateBr(m_returnBB);
        }
    }
    else
    {
        if (m_returnExpr)
        {
            m_returnExpr->codegen();
            llvm::Value* retVal = m_returnExpr->getLValue();
            codegenCompletionsStatements();
            builder->CreateRet(retVal);
        }
        else
        {
            codegenCompletionsStatements();
            builder->CreateRetVoid();
        }
    }
}

void IfAST::codegen()
{
    LLVMManager& manager = LLVMManager::getInstance();
    auto builder = manager.getBuilder();
    auto context = manager.getContext();

    m_ifExpr->codegen();
    llvm::Value* condValue = m_ifExpr->getLValue();
    if (condValue->getType() != builder->getInt1Ty())
    {
        condValue = castType(condValue, builder->getInt1Ty());
    }

    llvm::Function* function = builder->GetInsertBlock()->getParent();

    llvm::BasicBlock* ifBB= llvm::BasicBlock::Create(*context, "ifblock", function);
    llvm::BasicBlock* elseifBBHelp = nullptr;
    llvm::BasicBlock* elseBB= nullptr;
    llvm::BasicBlock* mergeBB = llvm::BasicBlock::Create(*context, "mergeblock");
    if (!m_elseIfs.empty())
    {
        elseifBBHelp = llvm::BasicBlock::Create(*context, "elseifblockhelp");
        builder->CreateCondBr(condValue, ifBB, elseifBBHelp);
    }
    if (m_elseBlock)
    {
        elseBB = llvm::BasicBlock::Create(*context, "elseblock");
        if (m_elseIfs.empty())
        {
            builder->CreateCondBr(condValue, ifBB, elseBB);
        }
    }
    else if(!m_elseBlock && m_elseIfs.empty())
    {
        builder->CreateCondBr(condValue, ifBB, mergeBB);
    }
    
    //generate ifBlock
    builder->SetInsertPoint(ifBB);
    m_ifBlock->addStatement(std::make_shared<GotoAST>(mergeBB));
    m_ifBlock->codegen();
    ifBB = builder->GetInsertBlock();

    //generate elseIfBlocks
    if (!m_elseIfs.empty())
    {
        llvm::BasicBlock* nextElseifBBHelp = nullptr;
        for (size_t i = 0; i<m_elseIfs.size();i++)
        {
            builder->SetInsertPoint(elseifBBHelp);
            function->insert(function->end(), elseifBBHelp);


            if (i == m_elseIfs.size() - 1)
            {
                if (m_elseBlock)
                {
                    nextElseifBBHelp = elseBB;
                }
                else
                {
                    nextElseifBBHelp = mergeBB;

                }
            }
            else
            {
                nextElseifBBHelp = llvm::BasicBlock::Create(*context, "elseifblockhelp");
            }

            llvm::BasicBlock* elseifBB = llvm::BasicBlock::Create(*context, "elseifblock");
            function->insert(function->end(), elseifBB);

            m_elseIfs[i].first->codegen();
            llvm::Value* elseifCondValue = m_elseIfs[i].first->getLValue();
            if (elseifCondValue->getType() != builder->getInt1Ty())
            {
                elseifCondValue = castType(elseifCondValue, builder->getInt1Ty());
            }
            builder->CreateCondBr(elseifCondValue, elseifBB, nextElseifBBHelp);
            elseifBBHelp = builder->GetInsertBlock();

            builder->SetInsertPoint(elseifBB);
            m_elseIfs[i].second->addStatement(std::make_shared<GotoAST>(mergeBB));
            m_elseIfs[i].second->codegen();
            elseifBB = builder->GetInsertBlock();

            elseifBBHelp = nextElseifBBHelp;
        }
    }
    

    //generate elseBlock
    if (m_elseBlock)
    {
        function->insert(function->end(), elseBB);
        builder->SetInsertPoint(elseBB);

        m_elseBlock->addStatement(std::make_shared<GotoAST>(mergeBB));
        m_elseBlock->codegen();
    }
    function->insert(function->end(), mergeBB);
    builder->SetInsertPoint(mergeBB);
    llvmValue = nullptr;
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
        llvmValue = builder->CreateIntCast(value,type,false,"i1toi32tmp");
    }
    else
    {
        throw std::runtime_error("ERROR::AST::Invalid Cast type!");
    }
}

void GotoAST::codegen()
{
    LLVMManager& manager = LLVMManager::getInstance();
    auto builder = manager.getBuilder();
    if (m_gotoelseBB && m_value)
    {
        builder->CreateCondBr(m_value, m_gotoBB, m_gotoelseBB);
    }
    else
    {
        builder->CreateBr(m_gotoBB);
    }
    llvmValue = m_value;
}

void WhileAST::codegen()
{
    LLVMManager& manager = LLVMManager::getInstance();
    auto builder = manager.getBuilder();
    auto context = manager.getContext();

    llvm::Function* function = builder->GetInsertBlock()->getParent();

    llvm::BasicBlock* exprBB = llvm::BasicBlock::Create(*context, "whileexprblock");
    llvm::BasicBlock* whileBB = llvm::BasicBlock::Create(*context, "whileblock");
    llvm::BasicBlock* mergeBB = llvm::BasicBlock::Create(*context, "mergeblock");
    
    //generate exprblock
    function->insert(function->end(), exprBB);
    builder->CreateBr(exprBB);
    builder->SetInsertPoint(exprBB);
    m_whileExpr->codegen();
    llvm::Value* value = m_whileExpr->getLValue();
    builder->CreateCondBr(value, whileBB, mergeBB);

    //generate whileblock
    function->insert(function->end(), whileBB);
    builder->SetInsertPoint(whileBB);
    for (const auto& brk : m_whileBlock->getBreaks())
    {
        brk->setNextBlock(mergeBB);
    }
    for (const auto& cont : m_whileBlock->getContinuations())
    {
        cont->setNextBlock(exprBB);
    }
    m_whileBlock->codegen();
    builder->CreateBr(exprBB);

    //continue block
    function->insert(function->end(), mergeBB);
    builder->SetInsertPoint(mergeBB);
}

void BreakAST::codegen()
{
    LLVMManager& manager = LLVMManager::getInstance();
    auto builder = manager.getBuilder();
    codegenCompletionsStatements();
    builder->CreateBr(m_nextBlock);
}

void ContinueAST::codegen()
{
    LLVMManager& manager = LLVMManager::getInstance();
    auto builder = manager.getBuilder();
    codegenCompletionsStatements();
    builder->CreateBr(m_nextBlock);
}