#pragma once

#include"AST.h"
#include"VarDeclAST.h"
#include"BlockAST.h"
#include"../SymbolTable.h"

class FunctionAST : public ASTNode, public std::enable_shared_from_this<FunctionAST> {
public:
    FunctionAST() = delete;
    FunctionAST(const std::string& name, TokenType retType,
        std::vector<std::shared_ptr<VarDeclAST>> args,
        std::shared_ptr<BlockAST> body, std::shared_ptr<SymbolTable> prevSymbolTable)
        : m_returnType(getType(retType)), m_args(args), m_body(std::move(body)), m_prevSymbolTable(prevSymbolTable)
    {
        setName(name);
    }

    void doSemantic() override;
    void codegen() override;
    llvm::Type* getReturnType();
    std::vector<std::shared_ptr<VarDeclAST>> getArgs();
private:
    llvm::Type* m_returnType;
    std::vector<std::shared_ptr<ReturnAST>> m_returns;
    std::vector<std::shared_ptr<VarDeclAST>> m_args;
    std::shared_ptr<BlockAST> m_body;
    std::shared_ptr<SymbolTable> m_prevSymbolTable;

};