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
llvm::Value* LLVMValueAST::codegen()
{
    return m_value;
}
llvm::Value* VarDeclAST::codegen()
{
    LLVMManager& manager = LLVMManager::getInstance();
    auto context = manager.getContext();
    auto builder = manager.getBuilder();
    auto symbolTable  = SymbolTableManager::getInstance().getSymbolTable();


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
        value = m_value->codegen();
    }

    builder->CreateStore(value, alloca);

    symbolTable->addVar(m_name, alloca);

    return alloca;
}

llvm::Value* VarExprAST::codegen()
{
    LLVMManager& manager = LLVMManager::getInstance();
    auto builder = manager.getBuilder();
    auto symbolTable = SymbolTableManager::getInstance().getSymbolTable();

    llvm::Value* var = symbolTable->getValueVar(m_name);
    return var;
}

llvm::Value* AssignExprAST::codegen()
{
    LLVMManager& manager = LLVMManager::getInstance();
    auto builder = manager.getBuilder();
    auto symbolTable = SymbolTableManager::getInstance().getSymbolTable();

    llvm::Value* var = symbolTable->getPtrVar(m_varName);
    llvm::Value* value = m_val->codegen();
    if (!value) {
        return nullptr;
    }
    builder->CreateStore(value, var);
    return value;
}

llvm::Value* LiteralExprAST::codegen()
{
    LLVMManager& manager = LLVMManager::getInstance();
    auto context = manager.getContext();
    auto builder = manager.getBuilder();

    if (m_type == TokenType::DoubleLiteral)
    {
        return llvm::ConstantFP::get(*context, llvm::APFloat(std::stod(m_value)));
    }
    else if (m_type == TokenType::IntLiteral)
    {
        return llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context), std::stoi(m_value));
    }
    else if (m_type == TokenType::TrueLiteral)
    {
        return llvm::ConstantInt::get(llvm::Type::getInt1Ty(*context), true);
    }
    else if (m_type == TokenType::FalseLiteral)
    {
        return llvm::ConstantInt::get(llvm::Type::getInt1Ty(*context), false);
    }
    else if (m_type == TokenType::StringLiteral)
    {
        llvm::Constant* globalString = builder->CreateGlobalStringPtr(m_value, ".str");
        llvm::Value* zero = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context), 0);
        return builder->CreateConstInBoundsGEP1_32(globalString->getType(), globalString, 0, "strptr");
    }
    return nullptr;
}

llvm::Value* BinaryExprAST::codegen()
{
    LLVMManager& manager = LLVMManager::getInstance();
    auto builder = manager.getBuilder();
    auto module = manager.getModule();
    auto context = manager.getContext();
    
    //for these operators, the code must be generated differently
    switch (m_op)
    {
    case TokenType::Or:
    {
        llvm::Value* lhsValue = m_lhs->codegen();
        lhsValue = castType(lhsValue, builder->getInt1Ty());

        llvm::Function* function = builder->GetInsertBlock()->getParent();

        llvm::BasicBlock* previousBB = builder->GetInsertBlock();
        llvm::BasicBlock* rhsBB = llvm::BasicBlock::Create(*context, "rhsblock");
        llvm::BasicBlock* mergeBB = llvm::BasicBlock::Create(*context, "mergeblock");


        builder->CreateCondBr(castType(lhsValue, builder->getInt1Ty()), mergeBB, rhsBB);

        function->insert(function->end(), rhsBB);
        builder->SetInsertPoint(rhsBB);
        llvm::Value* rhsValue = m_rhs->codegen();
        rhsValue = castType(rhsValue, builder->getInt1Ty());
        builder->CreateBr(mergeBB);
        rhsBB = builder->GetInsertBlock();

        function->insert(function->end(), mergeBB);
        builder->SetInsertPoint(mergeBB);

        llvm::PHINode* phiNode = builder->CreatePHI(builder->getInt1Ty(), 2, "orresult");
        phiNode->addIncoming(lhsValue, previousBB);
        phiNode->addIncoming(rhsValue, rhsBB);

        return phiNode;
    }
    case TokenType::And:
    {
        llvm::Value* lhsValue = m_lhs->codegen();
        lhsValue = castType(lhsValue, builder->getInt1Ty());

        llvm::Function* function = builder->GetInsertBlock()->getParent();

        llvm::BasicBlock* previousBB = builder->GetInsertBlock();
        llvm::BasicBlock* rhsBB = llvm::BasicBlock::Create(*context, "rhsblock");
        llvm::BasicBlock* mergeBB = llvm::BasicBlock::Create(*context, "mergeblock");


        builder->CreateCondBr(castType(lhsValue, builder->getInt1Ty()), rhsBB, mergeBB);

        function->insert(function->end(), rhsBB);
        builder->SetInsertPoint(rhsBB);
        llvm::Value* rhsValue = m_rhs->codegen();
        rhsValue = castType(rhsValue, builder->getInt1Ty());
        builder->CreateBr(mergeBB);
        rhsBB = builder->GetInsertBlock();

        function->insert(function->end(), mergeBB);
        builder->SetInsertPoint(mergeBB);

        llvm::PHINode* phiNode = builder->CreatePHI(builder->getInt1Ty(), 2, "orresult");
        phiNode->addIncoming(lhsValue, previousBB);
        phiNode->addIncoming(rhsValue, rhsBB);

        return phiNode;
    }
    }


    llvm::Value* lhsValue = m_lhs->codegen();
    llvm::Value* rhsValue = m_rhs->codegen();

    if (!lhsValue || !rhsValue) {
        return nullptr;
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
            return builder->CreateFAdd(lhsValue, rhsValue, "addtmp");
        }
        else if (lhsType->isIntegerTy())
        {
            return builder->CreateAdd(lhsValue, rhsValue, "addtmp");
        }
        else
        {
            throw std::runtime_error("ERROR::AST::Invalid type for binary operation " + lhsType->getStructName().str());
        }
    case TokenType::Minus:
        if (lhsType->isDoubleTy())
        {
            return builder->CreateFSub(lhsValue, rhsValue, "subtmp");
        }
        else if (lhsType->isIntegerTy())
        {
            return builder->CreateSub(lhsValue, rhsValue, "subtmp");
        }
        else
        {
            throw std::runtime_error("ERROR::AST::Invalid type for binary operation " + lhsType->getStructName().str());
        }
    case TokenType::Mul:
        if (lhsType->isDoubleTy())
        {
            return builder->CreateFMul(lhsValue, rhsValue, "multmp");
        }
        else if (lhsType->isIntegerTy())
        {
            return builder->CreateMul(lhsValue, rhsValue, "multmp");
        }
        else
        {
            throw std::runtime_error("ERROR::AST::Invalid type for binary operation " + lhsType->getStructName().str());
        }
    case TokenType::Div:
        if (lhsType->isDoubleTy())
        {
            return builder->CreateFDiv(lhsValue, rhsValue, "divtmp");
        }
        else if (lhsType->isIntegerTy())
        {
            return builder->CreateSDiv(lhsValue, rhsValue, "divtmp");
        }
        else
        {
            throw std::runtime_error("ERROR::AST::Invalid type for binary operation " + lhsType->getStructName().str());
        }
    case TokenType::Less:
        if (lhsType->isDoubleTy())
        {
            return builder->CreateFCmpULT(lhsValue, rhsValue, "lesstmp");
        }
        else if (lhsType->isIntegerTy())
        {
            return builder->CreateICmpSLT(lhsValue, rhsValue, "lesstmp");
        }
        else
        {
            throw std::runtime_error("ERROR::AST::Invalid type for binary operation " + lhsType->getStructName().str());
        }
    case TokenType::Greater:
        if (lhsType->isDoubleTy())
        {
            return builder->CreateFCmpUGT(lhsValue, rhsValue, "greatertmp");
        }
        else if (lhsType->isIntegerTy())
        {
            return builder->CreateICmpSGT(lhsValue, rhsValue, "greatertmp");
        }
        else
        {
            throw std::runtime_error("ERROR::AST::Invalid type for binary operation " + lhsType->getStructName().str());
        }
    case TokenType::Equal:
        if (lhsType->isDoubleTy())
        {
            return builder->CreateFCmpUEQ(lhsValue, rhsValue, "equaltmp");
        }
        else if (lhsType->isIntegerTy())
        {
            return builder->CreateICmpEQ(lhsValue, rhsValue, "equaltmp");
        }
        else
        {
            throw std::runtime_error("ERROR::AST::Invalid type for binary operation " + lhsType->getStructName().str());
        }
    case TokenType::BitAnd:
        return builder->CreateAnd(lhsValue, rhsValue, "bitandtmp");
    case TokenType::BitOr:
        return builder->CreateOr(lhsValue, rhsValue, "bitortmp");
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
            return castType(result, lhsType);
        }

        return result;
    }
    default:
        throw std::runtime_error("ERROR::AST::Invalid binary operator: " + g_nameTypes[static_cast<int>(m_op)]);
    }
    return nullptr;
}

llvm::Value* UnaryExprAST::codegen()
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
            return incrementResult;
        }
        else
        {
            return varValue;
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
            return decrementResult;
        }
        else
        {
            return varValue;
        }
        break;
    }
    }
    return nullptr;
}

llvm::Value* ConsoleOutputExprAST::codegen()
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
        value = m_expr->codegen();
    }

    std::string formatStr = "%s";
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
            llvm::Value* true_str = builder->CreateGlobalStringPtr("true\n");
            llvm::Value* false_str = builder->CreateGlobalStringPtr("false\n");
            llvm::Value* result = builder->CreateSelect(value, true_str, false_str);
            printfArgs.push_back(bool_str);
            printfArgs.push_back(result);
            return builder->CreateCall(printFunc, printfArgs);
        }
    }
    else
    {
        formatStr = "%s";
        printfArgs.push_back(builder->CreateGlobalStringPtr(formatStr));
        printfArgs.push_back(builder->CreateGlobalStringPtr("\n"));
        return builder->CreateCall(printFunc, printfArgs);
    }

    printfArgs.push_back(builder->CreateGlobalStringPtr(formatStr));
    printfArgs.push_back(value);


    return builder->CreateCall(printFunc, llvm::ArrayRef<llvm::Value* >(printfArgs));
}

llvm::Value* BlockAST::codegen() 
{
    LLVMManager& manager = LLVMManager::getInstance();
    auto builder = manager.getBuilder();

    llvm::Value* lastVal = nullptr;
    for (const auto& stmt : m_stmts) {
        lastVal = stmt->codegen();
        if (std::dynamic_pointer_cast<ReturnAST>(stmt))
        {
            break;
        }
    }
    llvm::BasicBlock* lastBB = builder->GetInsertBlock();
    if (lastBB->empty())
    {
        lastBB->removeFromParent();
    }

    return lastVal;
}

std::vector<std::shared_ptr<ReturnAST>> BlockAST::getReturns()
{
    std::vector<std::shared_ptr<ReturnAST>> returns;
    for (const auto& stmt : m_stmts)
    {
        std::shared_ptr<ReturnAST> returnAST = std::dynamic_pointer_cast<ReturnAST>(stmt);
        if (returnAST != nullptr)
        {
            returns.push_back(returnAST);
        }
        std::shared_ptr<BlockAST> blockAST = std::dynamic_pointer_cast<BlockAST>(stmt);
        if (blockAST != nullptr)
        {
            for (const auto& ret : blockAST->getReturns())
            {
                returns.push_back(ret);
            }
        }

        std::shared_ptr<IfAST> ifAST= std::dynamic_pointer_cast<IfAST>(stmt);
        if (ifAST != nullptr)
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

llvm::Value* ProtFunctionAST::codegen() 
{
    LLVMManager& manager = LLVMManager::getInstance();
    auto module = manager.getModule();
    std::vector<llvm::Type*> argTypes;
    for (const auto& arg : m_args) {
        argTypes.push_back(getType(arg.first));
    }
    if (!m_returnType)
    {
        throw std::runtime_error("ERROR::AST::The function declaration cannot be specified without the return value type");
    }
    llvm::FunctionType* funcType = llvm::FunctionType::get(m_returnType, argTypes, false);
    llvm::Function* func = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, m_name, module.get());

    unsigned int i = 0;
    for (auto& Arg : func->args())
        Arg.setName(m_args[i].second);
    return func;
}

llvm::Value* FunctionAST::codegen()
{
    LLVMManager& manager = LLVMManager::getInstance();
    auto context = manager.getContext();
    auto module = manager.getModule();
    auto builder = manager.getBuilder();
    auto symbolTableFunc = SymbolTableManager::getInstance().getSymbolTable();

    std::vector<llvm::Type*> argTypes;
    for (const auto& arg : m_args) {
        argTypes.push_back(getType(arg.first));
    }
    llvm::Function* func =nullptr;
    func = module->getFunction(m_name);
    llvm::BasicBlock* returnBB = nullptr;
    if (func == nullptr)
    {
        if (m_returns.size() > 1)
        {
            returnBB = llvm::BasicBlock::Create(*context, "returnblock");
            for (const auto& ret : m_returns)
            {
                ret->setReturnBB(returnBB);
            }
        }
        llvm::FunctionType* funcType = llvm::FunctionType::get(m_returnType, argTypes, false);
        func = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, m_name, module.get());
    }

    llvm::BasicBlock* entryBB = llvm::BasicBlock::Create(*context, "entry", func);
    builder->SetInsertPoint(entryBB);
    
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
        std::shared_ptr<VarDeclAST> var = std::make_shared<VarDeclAST>(m_args[i].second, std::make_shared<LLVMValueAST>(&arg), m_args[i].first);
        var->doSemantic();
        var->codegen();
        ++i;
    }

    m_body->extendSymbolTable(symbolTableFunc);
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
    return func;
}

llvm::Value* CallExprAST::codegen()
{
    LLVMManager& manager = LLVMManager::getInstance();
    auto module = manager.getModule();
    auto builder = manager.getBuilder();

    auto symbolTable = SymbolTableManager::getInstance().getSymbolTable();

    llvm::Function* func = module->getFunction(m_name);
    if (!func) {
        throw std::runtime_error("Function not found: " + m_name);
    }
    
    std::vector<llvm::Value*> args;
    for (const auto& arg : m_args) {
        args.push_back(arg->codegen());
    }

    return builder->CreateCall(func, args, "calltmp");
}

llvm::Value* ReturnAST::codegen()
{
    LLVMManager& manager = LLVMManager::getInstance();
    auto builder = manager.getBuilder();
    auto context = manager.getContext();

    if (m_returnBB)
    {
        if (m_returnExpr)
        {
            llvm::Value* returnValue = m_returnExpr->codegen();
            builder->CreateStore(returnValue, m_returnVar);
            builder->CreateBr(m_returnBB);
        }
        else
        {
            builder->CreateBr(m_returnBB);
        }
    }
    else
    {
        if (m_returnExpr)
        {
            llvm::Value* retVal = m_returnExpr->codegen();
            builder->CreateRet(retVal);
        }
        else
        {
            builder->CreateRetVoid();
        }
    }
    return nullptr;

}

llvm::Value* IfAST::codegen()
{
    LLVMManager& manager = LLVMManager::getInstance();
    auto builder = manager.getBuilder();
    auto context = manager.getContext();

    llvm::Value* condValue = m_ifExpr->codegen();
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
    llvm::Value* ifValue = m_ifBlock->codegen();
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


            llvm::Value* elseifCondValue = m_elseIfs[i].first->codegen();
            if (elseifCondValue->getType() != builder->getInt1Ty())
            {
                elseifCondValue = castType(elseifCondValue, builder->getInt1Ty());
            }
            builder->CreateCondBr(elseifCondValue, elseifBB, nextElseifBBHelp);
            elseifBBHelp = builder->GetInsertBlock();

            builder->SetInsertPoint(elseifBB);
            m_elseIfs[i].second->addStatement(std::make_shared<GotoAST>(mergeBB));
            llvm::Value* elseifValue = m_elseIfs[i].second->codegen();
            elseifBB = builder->GetInsertBlock();

            elseifBBHelp = nextElseifBBHelp;
        }
    }
    

    //generate elseBlock
    llvm::Value* elseValue = nullptr;
    if (m_elseBlock)
    {
        function->insert(function->end(), elseBB);
        builder->SetInsertPoint(elseBB);

        m_elseBlock->addStatement(std::make_shared<GotoAST>(mergeBB));
        elseValue = m_elseBlock->codegen();
    }
    function->insert(function->end(), mergeBB);
    builder->SetInsertPoint(mergeBB);
    return ifValue;
}

llvm::Value* CastAST::codegen()
{
    LLVMManager& manager = LLVMManager::getInstance();
    auto builder = manager.getBuilder();

    llvm::Value* value = m_value->codegen();
    llvm::Type* type = getType(m_type);
    
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
    else if (type->isIntegerTy() && value->getType()->isIntegerTy(1))
    {
        return builder->CreateIntCast(value,type,false,"i1toi32tmp");
    }
    else
    {
        throw std::runtime_error("ERROR::AST::Invalid Cast type!");
    }
}

llvm::Value* GotoAST::codegen()
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
    return m_value;
}

llvm::Value* WhileAST::codegen()
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
    llvm::Value* value = m_whileExpr->codegen();
    builder->CreateCondBr(value, whileBB, mergeBB);

    //generate whileblock
    function->insert(function->end(), whileBB);
    builder->SetInsertPoint(whileBB);
    m_whileBlock->codegen();
    builder->CreateBr(exprBB);

    //continue block
    function->insert(function->end(), mergeBB);
    builder->SetInsertPoint(mergeBB);

    return nullptr;
}