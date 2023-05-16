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
	Parser(TokenStream stream);
	void parse();
private:
	TokenStream m_tokenStream;
	std::shared_ptr<SymbolTable> m_globalSymbolTable;
	std::shared_ptr<llvm::LLVMContext> m_context;
	std::shared_ptr<llvm::Module> m_module;
	std::shared_ptr<llvm::IRBuilder<>> m_builder;

	Token check(std::vector<TokenType> types);
	Token checkType();
	std::shared_ptr<BlockAST> parseBlock();
	std::vector<std::shared_ptr<ASTNode>> parseStatement();
	std::shared_ptr<ASTNode> parseLiteral();
	std::shared_ptr<VarExprAST> parseVariable();
	std::shared_ptr<AssignExprAST> parseAssign();
	std::shared_ptr<ASTNode> parseFactor();
	std::shared_ptr<ASTNode> parseExponentiation();
	std::shared_ptr<ASTNode> parseMulDiv();
	std::shared_ptr<ASTNode> parsePlusMinus();
	std::shared_ptr<ASTNode> parseCompareExpr();
	std::shared_ptr<ASTNode> parseLogicalAndExpr();
	std::shared_ptr<ASTNode> Parser::parseLogicalOrExpr();
	std::shared_ptr<ASTNode> parseExpression();
	std::vector<std::shared_ptr<ASTNode>> parseVarDecl();
	std::shared_ptr<ASTNode> parseFunction();
	std::shared_ptr<CallExprAST> parseCallFunc();
	std::shared_ptr<ReturnAST> parseReturn();
	std::shared_ptr<ASTNode> parsePrint();
	std::vector<std::pair<TokenType, std::string>> parseArgs();
	std::shared_ptr<IfAST> parseIf();

};

