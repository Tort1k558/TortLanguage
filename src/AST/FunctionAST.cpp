#include"FunctionAST.h"

#include"CallExprAST.h"
#include"LLVMValueAST.h"

void FunctionAST::doSemantic()
{
    LLVMManager& manager = LLVMManager::getInstance();
    auto context = manager.getContext();
    auto symbolTable = SymbolTableManager::getInstance().getSymbolTable();
    for (const auto& arg : m_args) {
        arg->doSemantic();
    }
    m_body->doSemantic();

    m_returns = m_body->getReturns();
    if (!m_returnType)
    {
        if (!m_returns.empty())
        {
            std::vector<std::shared_ptr<ReturnAST>> returnRecursion;

            for (const auto& ret : m_returns)
            {
                //If the function in the return value calls itself, skip
                std::shared_ptr<CallExprAST> callExpr = std::dynamic_pointer_cast<CallExprAST>(ret->getReturnExpr());
                if (callExpr)
                {
                    if (callExpr->getNameCallFunction() == m_name)
                    {
                        returnRecursion.push_back(ret);
                        continue;
                    }
                }
                ret->doSemantic();
                m_returnType = ret->llvmType;
                symbolTable->addNode(shared_from_this());
            }

            if (returnRecursion.size() == m_returns.size())
            {
                throw std::runtime_error("ERROR::Could not determine the type of the returned function " + m_name + "!");
            }

            for (const auto& ret : returnRecursion)
            {
                ret->doSemantic();
            }
        }
        else
        {
            //if the function has no type and returns, we return void
            m_body->addStatement(std::make_shared<ReturnAST>(nullptr));
            m_returnType = llvm::Type::getVoidTy(*context);
            llvmType = m_returnType;
            return;
        }
    }
    else
    {
        if (m_returns.empty())
        {
            throw std::runtime_error("ERROR::The function " + m_name + " must return the value!");
        }
        symbolTable->addNode(shared_from_this());
        for (const auto& ret : m_returns)
        {
            ret->doSemantic();
        }
    }
    //Checking whether all return returns the same type
    m_returnType = m_returns[0]->llvmType;
    for (size_t i = 0; i < m_returns.size(); i++)
    {
        if (m_returns[i]->llvmType != m_returnType)
        {
            throw std::runtime_error("ERROR::The function " + m_name + " cannot return different types of values");
        }
    }
    symbolTable->addNode(shared_from_this());
    llvmType = m_returnType;
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
        if (arg->isReference())
        {
            argTypes.push_back(builder->getPtrTy());
        }
        else
        {
            argTypes.push_back(arg->llvmType);
        }
    }

    std::vector<std::shared_ptr<ASTNode>> args;
    for (const auto& arg : m_args)
    {
        args.push_back(arg);
    }
    //createFunction
    std::shared_ptr<ASTNode> funcAST = m_prevSymbolTable->getNode(m_name);
    llvm::Function* func = nullptr;
    if (funcAST)
    {
        if (funcAST->llvmValue)
        {
            func = llvm::dyn_cast<llvm::Function>(funcAST->llvmValue);
        }
    }
    if (!func)
    {
        llvm::FunctionType* funcType = llvm::FunctionType::get(m_returnType, argTypes, false);
        func = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, m_name, module.get());
        m_prevSymbolTable->addNode(shared_from_this());
        symbolTableFunc->addNode(shared_from_this());
    }
    llvmValue = func;

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
        arg.setName("arg" + std::to_string(i));
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
}

llvm::Type* FunctionAST::getReturnType()
{
    return m_returnType;
}

std::vector<std::shared_ptr<VarDeclAST>> FunctionAST::getArgs()
{
    return m_args;
}