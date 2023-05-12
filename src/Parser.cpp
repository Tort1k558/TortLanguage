#include "Parser.h"

#include<stdexcept>
#include<memory>
#include<iostream>

using namespace llvm;
Parser::Parser(TokenStream *stream) :
	m_tokenStream(*stream),
	m_globalSymbolTable(std::make_shared<SymbolTable>())
{
	parse();
	LLVMManager& manager = LLVMManager::getInstance();
	m_context = manager.getContext();
	m_module = manager.getModule();
	m_builder = manager.getBuilder();
}
void Parser::parse()
{
	SymbolTableManager::getInstance().setSymbolTable(m_globalSymbolTable);
	while (m_tokenStream->type != TokenType::EndOfFile)
	{
		switch (m_tokenStream->type)
		{
		case TokenType::Def:
			parseFunction();
			break;
		case TokenType::Int:
		case TokenType::Double:
		case TokenType::Bool:
			m_tokenStream++;
			break;
		default:
			std::cerr << "ERROR::PARSER::Unknown Token: " << g_nameTypes[static_cast<int>(m_tokenStream->type)] << std::endl;
			m_tokenStream++;
			break;
		}
	}
}
std::shared_ptr<ASTNode> Parser::parseLiteral()
{
	switch (m_tokenStream->type)
	{
	case TokenType::IntNumber:
	{
		std::string value = m_tokenStream->value;
		m_tokenStream++;
		return std::make_shared<LiteralExprAST<int>>(std::stoi(value));
	}
	case TokenType::DoubleNumber:
	{
		std::string value = m_tokenStream->value;
		m_tokenStream++;
		return std::make_shared<LiteralExprAST<double>>(std::stod(value));
	}
	}
}
std::shared_ptr<VarExprAST> Parser::parseVariable()
{
	std::string varName = check({TokenType::Identifier}).value;
	m_tokenStream++;
	return std::make_shared<VarExprAST>(varName);
}
std::shared_ptr<ASTNode> Parser::parseExpression()
{
	std::shared_ptr<ASTNode> lhs = parseTerm();

	while (m_tokenStream->type == TokenType::Plus || m_tokenStream->type == TokenType::Minus)
	{
		Token opToken = *m_tokenStream;
		m_tokenStream++;

		std::shared_ptr<ASTNode> rhs = parseTerm();

		if (opToken.type == TokenType::Plus)
		{
			lhs = std::make_shared<BinaryExprAST>(TokenType::Plus, lhs, rhs);
		}
		else if (opToken.type == TokenType::Minus)
		{
			lhs = std::make_shared<BinaryExprAST>(TokenType::Minus, lhs, rhs);
		}
	}

	return parseCompareExpr(lhs);
}
std::shared_ptr<ASTNode> Parser::parseCompareExpr(std::shared_ptr<ASTNode> lhs)
{
	if (m_tokenStream->type == TokenType::Less || m_tokenStream->type == TokenType::Greater || m_tokenStream->type == TokenType::Equal)
	{
		TokenType op = m_tokenStream->type;
		m_tokenStream++;
		std::shared_ptr<ASTNode> rhs = parseTerm();
		lhs = std::make_shared<BinaryExprAST>(op, lhs, rhs);
		return lhs;
	}
	return lhs;
}
std::shared_ptr<ASTNode> Parser::parseTerm()
{
	std::shared_ptr<ASTNode> lhs = parseFactor();

	while (m_tokenStream->type == TokenType::Mul || m_tokenStream->type == TokenType::Div)
	{
		Token opToken = *m_tokenStream;
		m_tokenStream++;

		std::shared_ptr<ASTNode> rhs = parseFactor();

		if (opToken.type == TokenType::Mul)
		{
			lhs = std::make_shared<BinaryExprAST>(TokenType::Mul, lhs, rhs);
		}
		else if (opToken.type == TokenType::Div)
		{
			lhs = std::make_shared<BinaryExprAST>(TokenType::Div, lhs, rhs);
		}
	}

	return lhs;
}

std::shared_ptr<ASTNode> Parser::parseFactor()
{
	if (m_tokenStream->type == TokenType::OpenParen)
	{
		m_tokenStream++;
		std::shared_ptr<ASTNode> expression = parseExpression();
		check({ TokenType::CloseParen });
		m_tokenStream++;
		return expression;
	}
	else if (m_tokenStream->type == TokenType::Identifier)
	{
		if (m_tokenStream.next().type == TokenType::OpenParen)
		{
			return parseCallFunc();
		}
		return parseVariable();
	}
	else if (m_tokenStream->type == TokenType::IntNumber || m_tokenStream->type == TokenType::DoubleNumber)
	{
		return parseLiteral();
	}
	else
	{
		std::cerr << "ERROR::PARSER::Unexpected Token: " << g_nameTypes[static_cast<int>(m_tokenStream->type)] << std::endl;
		return nullptr;
	}
}
std::shared_ptr<AssignExprAST> Parser::parseAssign()
{
	std::shared_ptr<ASTNode> val;
	std::string varName = check({ TokenType::Identifier }).value;
	m_tokenStream++;
	check({ TokenType::Assign });
	m_tokenStream++;
	val = parseExpression();
	return std::make_shared<AssignExprAST>(varName, val);
}
std::shared_ptr<ASTNode> Parser::parseStatement()
{
	switch (m_tokenStream->type)
	{
	case TokenType::Def:
		m_tokenStream++;
		parseFunction();
		break;
	case TokenType::Int:
	case TokenType::Double:
	case TokenType::Bool:
		if (m_tokenStream.next().type == TokenType::Identifier)
		{
			return parseVarDecl()[0];
		}
		break;
	case TokenType::Identifier:
		if (m_tokenStream.next().type == TokenType::Assign)
		{
			return parseAssign();
		}
		if (m_tokenStream.next().type == TokenType::OpenParen)
		{
			return parseCallFunc();
		}
		return parseExpression();
	case TokenType::Return:
		return parseReturn();
	default:
		m_tokenStream++;
		break;
	}
}
std::vector<std::shared_ptr<VarDeclAST>> Parser::parseVarDecl()
{
	std::vector<std::shared_ptr<VarDeclAST>> vars;
	std::vector<std::pair<std::string, std::shared_ptr<ASTNode>>> varsAssign;
	TokenType varType = checkType().type;
	m_tokenStream++;
	while (m_tokenStream->type != TokenType::Semicolon)
	{
		if (m_tokenStream->type == TokenType::Comma)
		{
			m_tokenStream++;
			continue;
		}
		else if (m_tokenStream->type == TokenType::Identifier && m_tokenStream.next().type == TokenType::Assign)
		{
			std::string varName = m_tokenStream->value;
			m_tokenStream++;
			m_tokenStream++;
			std::shared_ptr<ASTNode> value = parseExpression();
			varsAssign.push_back({ varName, value });
		}
		else if (m_tokenStream->type == TokenType::Identifier && m_tokenStream.next().type != TokenType::Assign)
		{
			std::string varName = m_tokenStream->value;
			m_tokenStream++;
			varsAssign.push_back({ varName, nullptr });
		}
	}
	for (const auto& assign : varsAssign)
	{
		vars.push_back(std::make_shared<VarDeclAST>(assign.first, assign.second, varType));
	}
	return vars;
}
std::shared_ptr<BlockAST> Parser::parseBlock()
{
	check({ TokenType::BlockStart });
	m_tokenStream++;
	auto block = std::make_unique<BlockAST>(m_globalSymbolTable);
	while(m_tokenStream->type != TokenType::BlockStop)
	{
		block->addStatement(parseStatement());
		check({ TokenType::Semicolon });
		m_tokenStream++;
	}
	
	check({ TokenType::BlockStop });
	m_tokenStream++;
	return block;
}
std::vector<std::pair<TokenType, std::string>> Parser::parseArgs()
{
	auto symbolTableFunc = SymbolTableManager::getInstance().getSymbolTable();
	std::vector<std::pair<TokenType, std::string>> args;
	check({ TokenType::OpenParen });
	m_tokenStream++;
	while(m_tokenStream->type != TokenType::CloseParen)
	{
		TokenType type = checkType().type;
		m_tokenStream++;
		std::string name = check({TokenType::Identifier}).value;
		m_tokenStream++;
		args.push_back({ type, name });
		symbolTableFunc->addVarType(name, getType(type));
		if (m_tokenStream->type != TokenType::CloseParen)
		{
			check({ TokenType::Comma });
			m_tokenStream++;
		}
	}
	check({ TokenType::CloseParen });
	m_tokenStream++;
	return args;
	
}
std::shared_ptr<CallExprAST> Parser::parseCallFunc()
{
	std::string nameFuncCall = check({TokenType::Identifier}).value;
	m_tokenStream++;
	std::vector<std::shared_ptr<ASTNode>> callArgs;
	check({ TokenType::OpenParen });
	m_tokenStream++;
	while(m_tokenStream->type != TokenType::CloseParen)
	{
		if (m_tokenStream->type == TokenType::Comma)
		{
			m_tokenStream++;
			continue;
		}
		std::shared_ptr<ASTNode> arg = parseExpression();
		callArgs.push_back(std::move(arg));
	}
	check({ TokenType::CloseParen });
	m_tokenStream++;
	return std::make_shared<CallExprAST>(nameFuncCall, std::move(callArgs));
}
std::shared_ptr<ASTNode> Parser::parseFunction()
{
	auto prevSymbolTable = SymbolTableManager::getInstance().getSymbolTable();

	auto symbolTableFunc = std::make_shared<SymbolTable>();
	symbolTableFunc->extend(prevSymbolTable.get());
	SymbolTableManager::getInstance().setSymbolTable(symbolTableFunc);

	check({ TokenType::Def });
	m_tokenStream++;
	TokenType retType = TokenType::Invalid;
	if (checkType().type != TokenType::Invalid)
	{
		retType = checkType().type;
		m_tokenStream++;
	}
	std::string funcName = check({ TokenType::Identifier }).value;
	m_tokenStream++;
	std::vector<std::pair<TokenType, std::string>> args = std::move(parseArgs());
	if (m_tokenStream->type == TokenType::Semicolon)
	{
		m_tokenStream++;
		auto proto = std::make_shared<ProtFunctionAST>(funcName, retType, args);
		proto->codegen();
		return proto;
	}
	std::shared_ptr<BlockAST> body = parseBlock();
	auto func = std::make_shared<FunctionAST>(funcName, retType, args, std::move(body));
	func->codegen();
	return func;

	SymbolTableManager::getInstance().setSymbolTable(prevSymbolTable);
}
std::shared_ptr<ReturnAST> Parser::parseReturn()
{
	m_tokenStream++;
	return std::make_shared<ReturnAST>(parseExpression());
}
Token Parser::check(std::vector<TokenType> types)
{
	Token curToken = *m_tokenStream;
	for (const auto& type : types)
	{
		if (curToken.type == type)
		{
			return curToken;
		}
	}
	return {TokenType::Invalid,};
}

Token Parser::checkType()
{
	return check({ TokenType::Int,TokenType::Double,TokenType::Bool });
}