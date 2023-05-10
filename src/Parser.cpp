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
	while (m_tokenStream->type != TokenType::EndOfFile)
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
			m_tokenStream++;
			break;
		default:
			m_tokenStream++;
			break;
		}
	}
}
std::shared_ptr<ASTNode> Parser::parseExpr()
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
	case TokenType::Identifier:
	{
		std::string varName = m_tokenStream->value;
		m_tokenStream++;
		return std::make_shared<VarExprAST>(varName);
	}
	case TokenType::OpenParen:
		m_tokenStream++;
		return parseExpr();
		check({ TokenType::CloseParen });
		m_tokenStream++;
	default:
		break;
	}
	return nullptr;
}

std::shared_ptr<AssignExprAST> Parser::parseAssign()
{
	std::shared_ptr<ASTNode> val;
	std::string varName = check({ TokenType::Identifier }).value;
	m_tokenStream++;
	check({ TokenType::Assign });
	m_tokenStream++;
	val = parseExpr();
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
		return parseExpr();
		break;
	default:
		m_tokenStream++;
		break;
	}
}
std::vector<std::shared_ptr<VarDeclAST>> Parser::parseVarDecl()
{
	std::vector<std::shared_ptr<VarDeclAST>> vars;
	std::vector<std::string> varNames;
	TokenType varType = checkType().type;
	m_tokenStream++;
	std::string name = check({ TokenType::Identifier }).value;
	m_tokenStream++;
	varNames.push_back(name);
	if (m_tokenStream->type == TokenType::Semicolon)
	{
		vars.push_back(std::make_shared<VarDeclAST>(name, 0.0, varType));
		return vars;
	}
	else if (m_tokenStream->type == TokenType::Comma)
	{
		while (m_tokenStream->type != TokenType::Semicolon)
		{
			
		}
	}
	for (size_t i = 0; i < varNames.size(); i++)
	{

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
		std::shared_ptr<ASTNode> arg = parseExpr();
		callArgs.push_back(std::move(arg));
	}
	check({ TokenType::CloseParen });
	m_tokenStream++;
	return std::make_shared<CallExprAST>(nameFuncCall, std::move(callArgs));
}
std::shared_ptr<FunctionAST> Parser::parseFunction()
{
	TokenType type = checkType().type;
	m_tokenStream++;
	std::string funcName = check({ TokenType::Identifier }).value;
	m_tokenStream++;
	std::vector<std::pair<TokenType, std::string>> args = std::move(parseArgs());
	std::shared_ptr<BlockAST> body = parseBlock();
	auto func = std::make_shared<FunctionAST>(funcName, type, args, std::move(body));
	func->codegen(m_globalSymbolTable);
	return func;
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
	std::cerr << "ERROR::PARSER::Expected Tokens:";
	for (const auto& token : types)
	{
		std::cerr << g_nameTypes[static_cast<int>(token)];
	}
	std::cerr << std::endl;
	return {};
}

Token Parser::checkType()
{
	return check({ TokenType::Int,TokenType::Double,TokenType::Bool });
}