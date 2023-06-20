#include"BinaryExprAST.h"

void BinaryExprAST::doSemantic()
{
    m_lhs->doSemantic();
    llvmType = m_lhs->llvmType;
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
    case TokenType::PlusAssign:
    {
        BinaryExprAST binaryExpr(TokenType::Plus, m_lhs, m_rhs);
        binaryExpr.doSemantic();
        binaryExpr.codegen();
        llvm::Value* value = binaryExpr.getLValue();
        builder->CreateStore(value, m_lhs->getRValue());
        llvmValue = value;
        return;
    }
    case TokenType::MinusAssign:
    {
        BinaryExprAST binaryExpr(TokenType::Minus, m_lhs, m_rhs);
        binaryExpr.doSemantic();
        binaryExpr.codegen();
        llvm::Value* value = binaryExpr.getLValue();
        builder->CreateStore(value, m_lhs->getRValue());
        llvmValue = value;
        return;
    }
    case TokenType::MulAssign:
    {
        BinaryExprAST binaryExpr(TokenType::Mul, m_lhs, m_rhs);
        binaryExpr.doSemantic();
        binaryExpr.codegen();
        llvm::Value* value = binaryExpr.getLValue();
        builder->CreateStore(value, m_lhs->getRValue());
        llvmValue = value;
        return;
    }
    case TokenType::DivAssign:
    {
        BinaryExprAST binaryExpr(TokenType::Div, m_lhs, m_rhs);
        binaryExpr.doSemantic();
        binaryExpr.codegen();
        llvm::Value* value = binaryExpr.getLValue();
        builder->CreateStore(value, m_lhs->getRValue());
        llvmValue = value;
        return;
    }
    case TokenType::ExponentiationAssign:
    {
        BinaryExprAST binaryExpr(TokenType::Exponentiation, m_lhs, m_rhs);
        binaryExpr.doSemantic();
        binaryExpr.codegen();
        llvm::Value* value = binaryExpr.getLValue();
        builder->CreateStore(value, m_lhs->getRValue());
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
        rhsValue = castType(rhsValue, lhsType);
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
    case TokenType::LessOrEqual:
        if (lhsType->isDoubleTy())
        {
            llvmValue = builder->CreateFCmpOLE(lhsValue, rhsValue, "lesstmp");
        }
        else if (lhsType->isIntegerTy())
        {
            llvmValue = builder->CreateICmpSLE(lhsValue, rhsValue, "lesstmp");
        }
        else
        {
            throw std::runtime_error("ERROR::AST::Invalid type for binary operation " + lhsType->getStructName().str());
        }
        return;
    case TokenType::More:
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
    case TokenType::MoreOrEqual:
        if (lhsType->isDoubleTy())
        {
            llvmValue = builder->CreateFCmpOGE(lhsValue, rhsValue, "lesstmp");
        }
        else if (lhsType->isIntegerTy())
        {
            llvmValue = builder->CreateICmpSGE(lhsValue, rhsValue, "lesstmp");
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

        llvm::Function* powFunction = llvm::Intrinsic::getDeclaration(module.get(), llvm::Intrinsic::pow, { lhsValue->getType() });
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