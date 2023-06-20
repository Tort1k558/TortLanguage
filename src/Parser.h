#pragma once

#include<map>
#include<memory>

#include<llvm/IR/IRBuilder.h>
#include<llvm/IR/IntrinsicsXCore.h>
#include<llvm/IR/LLVMContext.h>

#include"AST/AST.h"
#include"AST/VarExprAST.h"
#include"AST/UnaryExprAST.h"
#include"AST/CastAST.h"
#include"AST/CallExprAST.h"
#include"AST/ReturnAST.h"
#include"AST/IfAST.h"
#include"AST/WhileAST.h"
#include"AST/IndexExprAST.h"
#include"AST/LiteralExprAST.h"
#include"AST/BinaryExprAST.h"
#include"AST/ConsoleOutputExprAST.h"
#include"AST/ProtFunctionAST.h"
#include"AST/FunctionAST.h"
#include"AST/BlockAST.h"
#include"SymbolTable.h"
#include"TokenStream.h"
#include"Token.h"

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
	void eat(TokenType type);
	Token checkType();
	Token checkLiteral();
	Token checkUnary();
	std::shared_ptr<BlockAST> parseBlock();
	std::shared_ptr<VarExprAST> parseVariable();
	std::shared_ptr<UnaryExprAST> parseUnary(bool prefix);
	std::shared_ptr<CastAST> parseCast();
	std::shared_ptr<CallExprAST> parseCallFunc();
	std::shared_ptr<ReturnAST> parseReturn();
	std::shared_ptr<IfAST> parseIf();
	std::shared_ptr<WhileAST> parseWhile();
	std::shared_ptr<BreakAST> parseBreak();
	std::shared_ptr<ContinueAST> parseContinue();
	std::shared_ptr<IndexExprAST> parseIndex();
	std::shared_ptr<LiteralExprAST> parseLiteral();
	std::shared_ptr<ASTNode> parseFactor();
	std::shared_ptr<ASTNode> parseExponentiation();
	std::shared_ptr<ASTNode> parseMulDiv();
	std::shared_ptr<ASTNode> parsePlusMinus();
	std::shared_ptr<ASTNode> parseCompareExpr();
	std::shared_ptr<ASTNode> parseBitOrExpr();
	std::shared_ptr<ASTNode> parseBitAndExpr();
	std::shared_ptr<ASTNode> parseLogicalAndExpr();
	std::shared_ptr<ASTNode> parseLogicalOrExpr();
	std::shared_ptr<ASTNode> parseAssign();
	std::shared_ptr<ASTNode> parseExpression();
	std::shared_ptr<ASTNode> parseFunction();
	std::shared_ptr<ASTNode> parsePrint();
	std::vector<std::shared_ptr<ASTNode>> parseVarDecl();
	std::vector<std::shared_ptr<VarDeclAST>> parseArgs();
	std::vector<std::shared_ptr<ASTNode>> parseStatement();
};

