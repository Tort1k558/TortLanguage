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
template class VarDeclAST<int>;
template class VarDeclAST<double>;
template class VarDeclAST<bool>;
template<typename T>
llvm::Value* VarDeclAST<T>::codegen(llvm::LLVMContext* context, llvm::IRBuilder<>* builder, llvm::Module* module)
{
    llvm::Type* type = getType(context, m_type);
    llvm::AllocaInst* alloca = builder->CreateAlloca(type, nullptr, m_name.c_str());

    llvm::Value* val = nullptr;
    if (type->isDoubleTy())
    {
        val = llvm::ConstantFP::get(*context, llvm::APFloat(static_cast<double>(m_value)));
    }
    else if (type->isIntegerTy())
    {
        val = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context), static_cast<int>(m_value));
    }
    else if (type->isIntegerTy(1))
    {
        val = llvm::ConstantInt::get(llvm::Type::getInt1Ty(*context), static_cast<int>(m_value));
    }
    else
    {
        std::cout << "ERROR::This type is not supported yet" << std::endl;
    }
    builder->CreateStore(val, alloca);

    Table::symbolTable[m_name] = alloca;

    return alloca;
}

llvm::Value* VariableExprAST::codegen(llvm::LLVMContext* context, llvm::IRBuilder<>* builder, llvm::Module* module)
{
    llvm::Value* inst = Table::symbolTable[m_name];
    if (!inst) {
        std::cerr << "ERROR::Unknown variable name: " << m_name << std::endl;
        return nullptr;
    }
    llvm::Type* varType;
    llvm::Value* var;
    if (inst->getType()->isPointerTy())
    {
        varType = llvm::dyn_cast<llvm::AllocaInst>(inst)->getAllocatedType();
        var = builder->CreateLoad(varType, inst);
    }
    else
    {
        varType = inst->getType();
        var = inst;
    }
    return var;
}

llvm::Value* AssignExprAST::codegen(llvm::LLVMContext* context, llvm::IRBuilder<>* builder, llvm::Module* module)
{
    llvm::Value* var = Table::symbolTable[m_varName];
    if (!var) {
        std::cerr << "ERROR::Var is not defined" << std::endl;
    }
    llvm::Value* val = m_val->codegen(context, builder, module);
    if (!val) {
        return nullptr;
    }
    builder->CreateStore(val, var);
}

template class NumberExprAST<int>;
template class NumberExprAST<double>;
template class NumberExprAST<bool>;
template<typename T>
llvm::Value* NumberExprAST<T>::codegen(llvm::LLVMContext* context, llvm::IRBuilder<>* builder, llvm::Module* module)
{
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

llvm::Value* BinaryExprAST::codegen(llvm::LLVMContext* context, llvm::IRBuilder<>* builder, llvm::Module* module)
{
    llvm::Value* lhsVal = m_lhs->codegen(context, builder, module);
    llvm::Value* rhsVal = m_rhs->codegen(context, builder, module);
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
    default:
        std::cerr << "ERROR::Invalid binary operator: " << g_nameTypes[static_cast<int>(m_op)] << std::endl;
        return nullptr;
    }
}

llvm::Value* ConsoleOutputExprAST::codegen(llvm::LLVMContext* context, llvm::IRBuilder<>* builder, llvm::Module* module)
{
    static llvm::Function* printFunc;

    if (!printFunc)
    {
        llvm::FunctionType* printFuncType = llvm::FunctionType::get(llvm::IntegerType::getInt32Ty(*context), llvm::PointerType::get(llvm::IntegerType::getInt8Ty(*context), 0), true);
        printFunc = llvm::Function::Create(printFuncType, llvm::Function::ExternalLinkage, "printf", module);
    }
    if (!printFunc) {
        std::cerr << "Printf function not found" << std::endl;
        return nullptr;
    }

    llvm::Value* val = m_expr->codegen(context, builder, module);
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

llvm::Value* BlockAST::codegen(llvm::LLVMContext* context, llvm::IRBuilder<>* builder, llvm::Module* module) {
    llvm::Value* lastVal = nullptr;
    for (const auto& stmt : m_stmts) {
        lastVal = stmt->codegen(context, builder, module);
    }
    return lastVal;
}

llvm::Value* FunctionAST::codegen(llvm::LLVMContext* context, llvm::IRBuilder<>* builder, llvm::Module* module)
{
    std::vector<llvm::Type*> argTypes;
    for (const auto& arg : m_args) {
        argTypes.push_back(getType(context,arg.first));
    }

    llvm::FunctionType* funcType = llvm::FunctionType::get(getType(context, m_retType), argTypes, false);

    llvm::Function* func = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, m_name, module);

    llvm::BasicBlock* block = llvm::BasicBlock::Create(*context, "entry", func);
    builder->SetInsertPoint(block);

    unsigned i = 0;
    for (auto& arg : func->args()) {
        arg.setName("arg"+std::to_string(i));
        llvm::AllocaInst* argAlloc = builder->CreateAlloca(arg.getType(),nullptr,m_args[i].second);
        builder->CreateStore(&arg, argAlloc);
        Table::symbolTable.emplace(m_args[i].second, argAlloc);
        ++i;
    }

    m_body->codegen(context, builder, module);

    if (builder->GetInsertBlock()->getTerminator() == nullptr) {
        builder->CreateRet(llvm::Constant::getNullValue(getType(context, m_retType)));
    }

    return func;
}

llvm::Value* CallExprAST::codegen(llvm::LLVMContext* context, llvm::IRBuilder<>* builder, llvm::Module* module)
{
    llvm::Function* func = module->getFunction(m_name);
    if (!func) {
        std::cout << "Function not found" << std::endl;
    }

    std::vector<llvm::Value*> args;
    for (const auto& arg : m_args) {
        args.push_back(arg->codegen(context, builder, module));
    }

    return builder->CreateCall(func, args, "calltmp");
}

llvm::Value* ReturnAST::codegen(llvm::LLVMContext* context, llvm::IRBuilder<>* builder, llvm::Module* module)
{
    llvm::Value* retVal = m_retExpr->codegen(context,builder,module);
    builder->CreateRet(retVal);
    return retVal;
}