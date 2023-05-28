#include "Parser.h"

#include<stdexcept>
#include<memory>
#include<iostream>

using namespace llvm;
Parser::Parser(TokenStream stream) :
	m_tokenStream(stream),
	m_globalSymbolTable(std::make_shared<SymbolTable>())
{}
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
		default:
			throw std::runtime_error("ERROR::PARSER::Unknown Token: " + g_nameTypes[static_cast<int>(m_tokenStream->type)]);
			m_tokenStream++;
			break;
		}
	}
}

std::shared_ptr<ASTNode> Parser::parseLiteral()
{
	switch (m_tokenStream->type)
	{
	case TokenType::IntLiteral:
	{
		std::string value = m_tokenStream->value;
		m_tokenStream++;
		return std::make_shared<LiteralExprAST<int>>(std::stoi(value));
	}
	case TokenType::DoubleLiteral:
	{
		std::string value = m_tokenStream->value;
		m_tokenStream++;
		return std::make_shared<LiteralExprAST<double>>(std::stod(value));
	}
	case TokenType::TrueLiteral:
	{
		m_tokenStream++;
		return std::make_shared<LiteralExprAST<bool>>(true);
	}
	case TokenType::FalseLiteral:
	{
		m_tokenStream++;
		return std::make_shared<LiteralExprAST<bool>>(false);
	}
	}
	return nullptr;
}

std::shared_ptr<VarExprAST> Parser::parseVariable()
{
	std::string varName = check({TokenType::Identifier}).value;
	m_tokenStream++;
	return std::make_shared<VarExprAST>(varName);
}

std::shared_ptr<UnaryExprAST> Parser::parseUnary(bool prefix)
{
	TokenType op;
	std::string varName;
	if (prefix)
	{
		op = m_tokenStream->type;
		eat(op);
		varName = m_tokenStream->value;
		eat(TokenType::Identifier);
	}
	else
	{
		varName = m_tokenStream->value;
		eat(TokenType::Identifier);
		op = m_tokenStream->type;
		eat(op);
	}
	return std::make_shared<UnaryExprAST>(op, varName,prefix);
}

std::shared_ptr<CastAST> Parser::parseCast()
{
	eat(TokenType::Cast);
	eat(TokenType::Less);
	TokenType castType = checkType().type;
	eat(castType);
	eat(TokenType::Greater);
	eat(TokenType::OpenParen);
	std::shared_ptr<ASTNode> node = parseExpression();
	eat(TokenType::CloseParen);

	return std::make_shared<CastAST>(node, castType);
}

std::shared_ptr<ASTNode> Parser::parseExpression()
{
	return parseLogicalOrExpr();
}
std::shared_ptr<ASTNode> Parser::parseLogicalOrExpr()
{
	std::shared_ptr<ASTNode> lhs = parseLogicalAndExpr();

	while (m_tokenStream->type == TokenType::Or)
	{
		TokenType op = m_tokenStream->type;
		m_tokenStream++;

		std::shared_ptr<ASTNode> rhs = parseLogicalAndExpr();
		lhs = std::make_shared<BinaryExprAST>(op, lhs, rhs);
	}

	return lhs;
}
std::shared_ptr<ASTNode> Parser::parseLogicalAndExpr()
{
	std::shared_ptr<ASTNode> lhs = parseCompareExpr();

	while (m_tokenStream->type == TokenType::And)
	{
		TokenType op = m_tokenStream->type;
		m_tokenStream++;

		std::shared_ptr<ASTNode> rhs = parseCompareExpr();
		lhs = std::make_shared<BinaryExprAST>(op, lhs, rhs);
	}

	return lhs;
}
std::shared_ptr<ASTNode> Parser::parseCompareExpr()
{
	std::shared_ptr<ASTNode> lhs = parsePlusMinus();
	while (m_tokenStream->type == TokenType::Less || m_tokenStream->type == TokenType::Greater || m_tokenStream->type == TokenType::Equal)
	{
		TokenType op = m_tokenStream->type;
		m_tokenStream++;
		std::shared_ptr<ASTNode> rhs = parsePlusMinus();
		lhs = std::make_shared<BinaryExprAST>(op, lhs, rhs);
		return lhs;
	}
	return lhs;
}
std::shared_ptr<ASTNode> Parser::parsePlusMinus()
{
	std::shared_ptr<ASTNode> lhs = parseMulDiv();

	while (m_tokenStream->type == TokenType::Plus || m_tokenStream->type == TokenType::Minus)
	{
		TokenType op = m_tokenStream->type;
		m_tokenStream++;

		std::shared_ptr<ASTNode> rhs = parseMulDiv();

		lhs = std::make_shared<BinaryExprAST>(op, lhs, rhs);
	}

	return lhs;
}
std::shared_ptr<ASTNode> Parser::parseMulDiv()
{
	std::shared_ptr<ASTNode> lhs = parseExponentiation();

	while (m_tokenStream->type == TokenType::Mul || m_tokenStream->type == TokenType::Div)
	{
		TokenType op = m_tokenStream->type;
		m_tokenStream++;

		std::shared_ptr<ASTNode> rhs = parseExponentiation();

		lhs = std::make_shared<BinaryExprAST>(op, lhs, rhs);
	}

	return lhs;
}
std::shared_ptr<ASTNode> Parser::parseExponentiation()
{
	std::shared_ptr<ASTNode> lhs = parseFactor();

	while (m_tokenStream->type == TokenType::Exponentiation)
	{
		TokenType op = m_tokenStream->type;
		m_tokenStream++;

		std::shared_ptr<ASTNode> rhs = parseFactor();

		lhs = std::make_shared<BinaryExprAST>(op, lhs, rhs);
	}

	return lhs;
}
std::shared_ptr<ASTNode> Parser::parseFactor()
{
	if (m_tokenStream->type == TokenType::OpenParen)
	{
		eat(TokenType::OpenParen);
		std::shared_ptr<ASTNode> expression = parseExpression();
		eat(TokenType::CloseParen);
		return expression;
	}
	else if (m_tokenStream->type == TokenType::Identifier)
	{
		if (m_tokenStream.next().type == TokenType::OpenParen)
		{
			return parseCallFunc();
		}
		else if (m_tokenStream.next().type == TokenType::Increment || m_tokenStream.next().type == TokenType::Decrement)
		{
			return parseUnary(false);
		}
		return parseVariable();
	}
	else if (m_tokenStream->type == TokenType::Increment || m_tokenStream->type == TokenType::Decrement)
	{
		return parseUnary(true);
	}
	else if (checkLiteral().type != TokenType::Invalid)
	{
		return parseLiteral();
	}
	else if (m_tokenStream->type == TokenType::Cast)
	{
		return parseCast();
	}
	else
	{
		return nullptr;
	}
}

std::shared_ptr<AssignExprAST> Parser::parseAssign()
{
	std::string varName = check({ TokenType::Identifier }).value;
	m_tokenStream++;
	check({ TokenType::Assign });
	m_tokenStream++;
	return std::make_shared<AssignExprAST>(varName, parseExpression());
}

std::vector<std::shared_ptr<ASTNode>> Parser::parseStatement()
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
			return parseVarDecl();
		}
		break;
	case TokenType::Identifier:
		if (m_tokenStream.next().type == TokenType::Assign)
		{
			return { parseAssign() };
		}
		if (m_tokenStream.next().type == TokenType::OpenParen)
		{
			return { parseCallFunc() };
		}
		return { parseExpression() };
	case TokenType::Return:
		return { parseReturn() };
	case TokenType::Print:
		return { parsePrint() };
	case TokenType::If:
		return { parseIf() };
	default:
		return { parseExpression() };
		break;
	}
	return {};
}

std::shared_ptr<ASTNode> Parser::parsePrint()
{
	m_tokenStream++;

	check({ TokenType::OpenParen });
	m_tokenStream++;
	std::shared_ptr<ConsoleOutputExprAST> outAST = std::make_shared<ConsoleOutputExprAST>(parseExpression());
	check({ TokenType::CloseParen });
	m_tokenStream++;

	return outAST;
}

std::vector<std::shared_ptr<ASTNode>> Parser::parseVarDecl()
{
	std::vector<std::shared_ptr<ASTNode>> vars;
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
		else if(m_tokenStream->type == TokenType::Identifier && m_tokenStream.next().type != TokenType::Assign)
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
	auto prevSymbolTable = SymbolTableManager::getInstance().getSymbolTable();
	auto symbolTableBlock = std::make_shared<SymbolTable>();
	symbolTableBlock->extend(prevSymbolTable.get());
	SymbolTableManager::getInstance().setSymbolTable(symbolTableBlock);

	check({ TokenType::BlockStart });
	m_tokenStream++;
	auto block = std::make_unique<BlockAST>(m_globalSymbolTable);
	while(m_tokenStream->type != TokenType::BlockStop)
	{
		auto statements = parseStatement();
		for (const auto& stmt : statements)
		{
			block->addStatement(stmt);
		}
		if (!statements.empty())
		{
			std::shared_ptr<IfAST> ifAST = std::dynamic_pointer_cast<IfAST>(statements[0]);
			if (ifAST)
			{
				continue;
			}
		}
		check({ TokenType::Semicolon });
		m_tokenStream++;
	}
	
	check({ TokenType::BlockStop });
	m_tokenStream++;

	SymbolTableManager::getInstance().setSymbolTable(prevSymbolTable);

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
	TokenType retType = TokenType::NonType;
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
		auto proto = std::make_shared<ProtFunctionAST>(funcName, getType(retType), args);
		proto->codegen();
		SymbolTableManager::getInstance().setSymbolTable(prevSymbolTable);
		prevSymbolTable->addFunctionReturnType(funcName, proto->llvmType);
		return proto;
	}
	std::shared_ptr<BlockAST> body = parseBlock();
	auto func = std::make_shared<FunctionAST>(funcName, getType(retType), args, std::move(body));
	func->doSemantic();
	func->codegen();
	SymbolTableManager::getInstance().setSymbolTable(prevSymbolTable);
	prevSymbolTable->addFunctionReturnType(funcName, func->llvmType);
	return func;

}

std::shared_ptr<ReturnAST> Parser::parseReturn()
{
	m_tokenStream++;
	if (m_tokenStream->type == TokenType::Semicolon)
	{
		return std::make_shared<ReturnAST>(nullptr);
	}
	return std::make_shared<ReturnAST>(parseExpression());
}


std::shared_ptr<IfAST> Parser::parseIf()
{
	eat(TokenType::If);
	std::shared_ptr<ASTNode> ifExpr = parseExpression();
	eat(TokenType::Colon);
	std::shared_ptr<BlockAST> ifBlock = parseBlock();
	std::shared_ptr<BlockAST> elseBlock = nullptr;
	std::vector<std::pair<std::shared_ptr<ASTNode>, std::shared_ptr<BlockAST>>> elseIfs;
	while (m_tokenStream->type == TokenType::Else && m_tokenStream.next().type == TokenType::If)
	{
		eat(TokenType::Else);
		eat(TokenType::If);

		std::shared_ptr<ASTNode> elseIfExpr = parseExpression();
		eat(TokenType::Colon);

		std::shared_ptr<BlockAST> elseIfBlock = parseBlock();
		elseIfs.push_back({elseIfExpr, elseIfBlock});
	}
	if (m_tokenStream->type == TokenType::Else)
	{
		eat(TokenType::Else);
		eat(TokenType::Colon);

		elseBlock = parseBlock();
	}
	return std::make_shared<IfAST>(ifExpr, ifBlock, elseBlock, std::move(elseIfs));
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
	return { TokenType::Invalid };
}

Token Parser::checkType()
{
	return check({ TokenType::Int,TokenType::Double,TokenType::Bool });
}

Token Parser::checkLiteral()
{
	return check({ TokenType::IntLiteral,TokenType::DoubleLiteral,TokenType::TrueLiteral,TokenType::FalseLiteral });
}
void Parser::eat(TokenType type)
{
	if (m_tokenStream->type != type)
	{
		throw std::runtime_error("ERROR::PARSER::Unknown Token: " + g_nameTypes[static_cast<int>(m_tokenStream->type)] + "\nExpected: " + g_nameTypes[static_cast<int>(type)]);
	}
	m_tokenStream++;
}