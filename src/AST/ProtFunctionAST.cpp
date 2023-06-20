#include"ProtFunctionAST.h"

void ProtFunctionAST::doSemantic()
{
    for (const auto& arg : m_args) {
        arg->doSemantic();
    }
    llvmType = m_returnType;
    m_prevSymbolTable->addNode(shared_from_this());
}

void ProtFunctionAST::codegen()
{
    LLVMManager& manager = LLVMManager::getInstance();
    auto module = manager.getModule();
    auto symbolTable = SymbolTableManager::getInstance().getSymbolTable();

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

    symbolTable->addNode(shared_from_this());
    llvmValue = func;
}

std::vector<std::shared_ptr<VarDeclAST>> ProtFunctionAST::getArgs()
{
    return m_args;
}

llvm::Type* ProtFunctionAST::getReturnType()
{
    return m_returnType;
}