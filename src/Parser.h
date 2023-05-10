#pragma once
#include"TokenStream.h"
#include"Token.h"
#include<map>
#include<memory>
#include<llvm/IR/IRBuilder.h>
#include<llvm/IR/IntrinsicsXCore.h>
#include<llvm/IR/LLVMContext.h>
#include"AST.h"
#include"SymbolTable.h"
class Parser
{
public:
	Parser(TokenStream *stream);
	void parse();
private:
	TokenStream m_tokenStream;
	std::shared_ptr<SymbolTable> m_globalSymbolTable;
	Token check(std::vector<TokenType> types);
	Token checkType();
	std::shared_ptr<BlockAST> parseBlock();
	std::shared_ptr<ASTNode> parseExpr();
	std::vector<std::shared_ptr<VarDeclAST>> parseVarDecl();
	std::shared_ptr<FunctionAST> parseFunction();
	std::shared_ptr<AssignExprAST> parseAssign();
	std::shared_ptr<CallExprAST> parseCallFunc();
	std::shared_ptr<ASTNode> Parser::parseStatement();
	std::vector<std::pair<TokenType, std::string>> parseArgs();
	std::shared_ptr<llvm::LLVMContext> m_context;
	std::shared_ptr<llvm::Module> m_module;
	std::shared_ptr<llvm::IRBuilder<>> m_builder;

};

