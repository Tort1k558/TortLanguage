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
    case TokenType::NonType:
        return nullptr;
    default:
        break;
    }
    std::cout << "ERROR::UnknownType::" << g_nameTypes[static_cast<int>(type)];
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

    llvm::Type* type = getType(m_type);
    llvm::AllocaInst* alloca = builder->CreateAlloca(type, nullptr, m_name.c_str());
    llvm::Value* val;
    if (m_value == nullptr)
    {
        if (type->isDoubleTy())
        {
            val = llvm::ConstantFP::get(*context, llvm::APFloat(0.0));
        }
        else if (type->isIntegerTy())
        {
            val = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context), 0);
        }
        else if (type->isIntegerTy(1))
        {
            val = llvm::ConstantInt::get(llvm::Type::getInt1Ty(*context), false);
        }
        else
        {
            std::cerr << "ERROR::This type is not supported yet" << std::endl;
        }
    }
    else
    {
        val = m_value->codegen();
    }
    builder->CreateStore(val, alloca);

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
    if (!var) {
        std::cerr << "ERROR::Var is not defined" << std::endl;
    }
    llvm::Value* val = m_val->codegen();
    if (!val) {
        return nullptr;
    }
    builder->CreateStore(val, var);
}

template class LiteralExprAST<int>;
template class LiteralExprAST<double>;
template class LiteralExprAST<bool>;
template<typename T>
llvm::Value* LiteralExprAST<T>::codegen()
{
    LLVMManager& manager = LLVMManager::getInstance();
    auto context = manager.getContext();

    if (std::is_same<T, double>::value)
    {
        return llvm::ConstantFP::get(*context, llvm::APFloat(static_cast<double>(m_value)));
    }
    else if (std::is_same<T, int>::value)
    {
        return llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context), m_value);
    }
    else if (std::is_same<T, bool>::value)
    {
        return llvm::ConstantInt::get(llvm::Type::getInt1Ty(*context), m_value);
    }
}

llvm::Value* BinaryExprAST::codegen()
{
    LLVMManager& manager = LLVMManager::getInstance();
    auto builder = manager.getBuilder();
    auto context = manager.getContext();
    auto module = manager.getModule();
    llvm::Value* lhsVal = m_lhs->codegen();
    llvm::Value* rhsVal = m_rhs->codegen();

    if (!lhsVal || !rhsVal) {
        return nullptr;
    }
    llvm::Type* lhsType = lhsVal->getType();
    llvm::Type* rhsType = rhsVal->getType();
    if (lhsType != rhsType)
    {
        if (lhsType->isDoubleTy() && rhsType->isIntegerTy())
        {
            rhsVal = builder->CreateSIToFP(rhsVal, lhsType, "sitofptmp");
        }
        else if(lhsType->isIntegerTy() && rhsType->isDoubleTy())
        {
            rhsVal = builder->CreateFPToSI(rhsVal, lhsType, "fptositmp");
        }
        else
        {
            std::cerr << "ERROR::Invalid type for binary operation " << lhsType <<" and "<<rhsType <<std::endl;
            return nullptr;
        }
    }
    switch (m_op) {
    case TokenType::Plus:
        if (lhsType->isDoubleTy())
        {
            return builder->CreateFAdd(lhsVal, rhsVal, "addtmp");
        }
        else if (lhsType->isIntegerTy())
        {
            return builder->CreateAdd(lhsVal, rhsVal, "addtmp");
        }
        else
        {
            std::cerr << "ERROR::Invalid type for binary operation " << lhsType << std::endl;
            return nullptr;
        }
    case TokenType::Minus:
        if (lhsType->isDoubleTy())
        {
            return builder->CreateFSub(lhsVal, rhsVal, "subtmp");
        }
        else if (lhsType->isIntegerTy())
        {
            return builder->CreateSub(lhsVal, rhsVal, "subtmp");
        }
        else
        {
            std::cerr << "ERROR::Invalid type for binary operation " << lhsType << std::endl;
            return nullptr;
        }
    case TokenType::Mul:
        if (lhsType->isDoubleTy())
        {
            return builder->CreateFMul(lhsVal, rhsVal, "multmp");
        }
        else if (lhsType->isIntegerTy())
        {
            return builder->CreateMul(lhsVal, rhsVal, "multmp");
        }
        else
        {
            std::cerr << "ERROR::Invalid type for binary operation " << lhsType << std::endl;
            return nullptr;
        }
    case TokenType::Div:
        if (lhsType->isDoubleTy())
        {
            return builder->CreateFDiv(lhsVal, rhsVal, "divtmp");
        }
        else if (lhsType->isIntegerTy())
        {
            return builder->CreateSDiv(lhsVal, rhsVal, "divtmp");
        }
        else
        {
            std::cerr << "ERROR::Invalid type for binary operation " << lhsType << std::endl;
            return nullptr;
        }
    case TokenType::Less:
        if (lhsType->isDoubleTy())
        {
            return builder->CreateFCmpULT(lhsVal, rhsVal, "lesstmp");
        }
        else if (lhsType->isIntegerTy())
        {
            return builder->CreateICmpSLT(lhsVal, rhsVal, "lesstmp");
        }
        else
        {
            std::cerr << "ERROR::Invalid type for binary operation " << lhsType << std::endl;
            return nullptr;
        }
    case TokenType::Greater:
        if (lhsType->isDoubleTy())
        {
            return builder->CreateFCmpUGT(lhsVal, rhsVal, "greatertmp");
        }
        else if (lhsType->isIntegerTy())
        {
            return builder->CreateICmpSGT(lhsVal, rhsVal, "greatertmp");
        }
        else
        {
            std::cerr << "ERROR::Invalid type for binary operation " << lhsType << std::endl;
            return nullptr;
        }
    case TokenType::Equal:
        if (lhsType->isDoubleTy())
        {
            return builder->CreateFCmpUEQ(lhsVal, rhsVal, "equaltmp");
        }
        else if (lhsType->isIntegerTy())
        {
            return builder->CreateICmpEQ(lhsVal, rhsVal, "equaltmp");
        }
        else
        {
            std::cerr << "ERROR::Invalid type for binary operation " << lhsType << std::endl;
            return nullptr;
        }
    case TokenType::And:
        return builder->CreateAnd(lhsVal, rhsVal, "andtmp");
    case TokenType::Or:
        return builder->CreateOr(lhsVal, rhsVal, "ortmp");
    case TokenType::Exponentiation:
        //TODO
        return builder->CreateCall(module->getFunction("pow"), {lhsVal, rhsVal});
    default:
        std::cerr << "ERROR::Invalid binary operator: " << g_nameTypes[static_cast<int>(m_op)] << std::endl;
        return nullptr;
    }
}

llvm::Value* ConsoleOutputExprAST::codegen()
{
    LLVMManager& manager = LLVMManager::getInstance();
    auto context = manager.getContext();
    auto module = manager.getModule();
    auto builder = manager.getBuilder();
    auto symbolTable = SymbolTableManager::getInstance().getSymbolTable();

    static llvm::Function* printFunc;
    if (!printFunc)
    {
        llvm::FunctionType* printFuncType = llvm::FunctionType::get(llvm::IntegerType::getInt32Ty(*context), llvm::PointerType::get(llvm::IntegerType::getInt8Ty(*context), 0), true);
        printFunc = llvm::Function::Create(printFuncType, llvm::Function::ExternalLinkage, "printf", *module);
    }
    if (!printFunc) {
        std::cerr << "Printf function not found" << std::endl;
        return nullptr;
    }

    llvm::Value* val = m_expr->codegen();
    if (!val) {
        return nullptr;
    }

    std::string formatStr;
    llvm::Type* i = val->getType();
    if (i->isFloatTy() || i->isDoubleTy()) {
        formatStr = "%f\n";
    }
    else if (i->isIntegerTy(1))
    {
        formatStr = "%s\n";
        llvm::Value* bool_str = builder->CreateGlobalStringPtr("%s");
        llvm::Value* true_str = builder->CreateGlobalStringPtr("true");
        llvm::Value* false_str = builder->CreateGlobalStringPtr("false");
        llvm::Value* result = builder->CreateSelect(val, true_str, false_str);
        std::vector<llvm::Value*> printf_args;
        printf_args.push_back(bool_str);
        printf_args.push_back(result);
        return builder->CreateCall(printFunc, printf_args);
    }
    else if (i->isIntegerTy(32))
    {
        formatStr = "%d\n";
    }

    std::vector<llvm::Value*> printfArgs;
    llvm::Value* formatStrConst = builder->CreateGlobalStringPtr(formatStr);
    printfArgs.push_back(formatStrConst);
    printfArgs.push_back(val);


    return builder->CreateCall(printFunc, llvm::ArrayRef<llvm::Value* >(printfArgs));
}

llvm::Value* BlockAST::codegen() 
{
    llvm::Value* lastVal = nullptr;
    for (const auto& stmt : m_stmts) {
        lastVal = stmt->codegen();
    }
    return lastVal;
}

std::shared_ptr<ASTNode> BlockAST::getReturn()
{
    for (const auto& stmt : m_stmts)
    {
        std::shared_ptr<ReturnAST> ret = std::dynamic_pointer_cast<ReturnAST>(stmt);
        if (ret != nullptr)
        {
            return ret;
        }
    }
    return nullptr;
}

llvm::Value* ProtFunctionAST::codegen() 
{
    LLVMManager& manager = LLVMManager::getInstance();
    auto module = manager.getModule();
    std::vector<llvm::Type*> argTypes;
    for (const auto& arg : m_args) {
        argTypes.push_back(getType(arg.first));
    }
    if (!m_retType)
    {
        std::cerr << "ERROR::PARSER::The function declaration cannot be specified without the return value type" << std::endl;
        return nullptr;
    }
    llvm::FunctionType* funcType = llvm::FunctionType::get(m_retType, argTypes, false);
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
    if (func == nullptr)
    {
        llvm::Type* retType;
        if (!m_retType)
        {
            auto retAST = m_body->getReturn();
            if (retAST)
            {
                if (retAST->llvmType)
                {
                    m_retType = retAST->llvmType;
                }
                else
                {
                    m_retType = llvm::Type::getVoidTy(*context);
                }
            }
            else
            {
                m_retType = llvm::Type::getVoidTy(*context);
            }
        }
        llvm::FunctionType* funcType = llvm::FunctionType::get(m_retType, argTypes, false);
        func = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, m_name, module.get());
    }

    llvm::BasicBlock* block = llvm::BasicBlock::Create(*context, "entry", func);
    builder->SetInsertPoint(block);
    unsigned int i = 0;
    for (auto& arg : func->args())
    {
        arg.setName("arg"+std::to_string(i));
        std::shared_ptr<VarDeclAST> var = std::make_shared<VarDeclAST>(m_args[i].second, std::make_shared<LLVMValueAST>(&arg), m_args[i].first);
        var->codegen();
        ++i;
    }

    m_body->extendSymbolTable(symbolTableFunc);
    m_body->codegen();
    
    if (builder->GetInsertBlock()->getTerminator() == nullptr) 
    {
        if (m_retType->isVoidTy())
        {
            builder->CreateRetVoid();
        }
        else
        {
            builder->CreateRet(llvm::Constant::getNullValue(m_retType));
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
        std::cout << "Function not found" << std::endl;
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
    std::shared_ptr<llvm::IRBuilder<>> builder = manager.getBuilder();

    llvm::Value* retVal = m_retExpr->codegen();
    builder->CreateRet(retVal);
    return retVal;
}