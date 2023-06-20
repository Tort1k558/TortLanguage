#include"CallExprAST.h"

#include"VarDeclAST.h"
#include"FunctionAST.h"
#include"ProtFunctionAST.h"

void CallExprAST::doSemantic()
{
    auto symbolTable = SymbolTableManager::getInstance().getSymbolTable();
    for (const auto& arg : m_args)
    {
        arg->doSemantic();
    }
    std::shared_ptr<FunctionAST> funcNode = std::dynamic_pointer_cast<FunctionAST>(symbolTable->getNode(m_name));
    if (funcNode)
    {
        llvmType = funcNode->getReturnType();
    }
    std::shared_ptr<ProtFunctionAST> protNode = std::dynamic_pointer_cast<ProtFunctionAST>(symbolTable->getNode(m_name));
    if (protNode)
    {
        llvmType = protNode->getReturnType();
    }
}

void CallExprAST::codegen()
{
    LLVMManager& manager = LLVMManager::getInstance();
    auto module = manager.getModule();
    auto builder = manager.getBuilder();

    auto symbolTable = SymbolTableManager::getInstance().getSymbolTable();

    llvm::Function* func = nullptr;
    std::vector<std::shared_ptr<VarDeclAST>> argsAST;

    std::shared_ptr<FunctionAST> funcNode = std::dynamic_pointer_cast<FunctionAST>(symbolTable->getNode(m_name));
    if (funcNode)
    {
        func = llvm::dyn_cast<llvm::Function>(funcNode->llvmValue);
        argsAST = funcNode->getArgs();
    }
    std::shared_ptr<ProtFunctionAST> protNode = std::dynamic_pointer_cast<ProtFunctionAST>(symbolTable->getNode(m_name));
    if (protNode)
    {
        func = llvm::dyn_cast<llvm::Function>(protNode->llvmValue);
        argsAST = protNode->getArgs();
    }

    std::vector<llvm::Value*> args;
    for (size_t i = 0; i < m_args.size(); i++)
    {
        m_args[i]->codegen();
        if (argsAST[i]->isReference())
        {
            args.push_back(m_args[i]->getRValue());
        }
        else
        {
            args.push_back(m_args[i]->getLValue());
        }
    }

    llvmValue = builder->CreateCall(func, args);
}

std::string CallExprAST::getNameCallFunction()
{
    return m_name;
}