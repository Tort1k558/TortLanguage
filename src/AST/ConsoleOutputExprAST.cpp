#include"ConsoleOutputExprAST.h"

void ConsoleOutputExprAST::doSemantic()
{
    LLVMManager& manager = LLVMManager::getInstance();
    auto builder = manager.getBuilder();
    if (m_expr)
    {
        m_expr->doSemantic();
    }
    llvmType = builder->getVoidTy();
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