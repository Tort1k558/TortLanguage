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
	std::string minus = "";
	if (m_tokenStream->type == TokenType::Minus)
	{
		minus = "-";
		eat(TokenType::Minus);
	}
	switch (m_tokenStream->type)
	{
	case TokenType::IntLiteral:
	{
		std::string value = m_tokenStream->value;
		eat(TokenType::IntLiteral);
		return std::make_shared<LiteralExprAST>(minus+value, TokenType::IntLiteral);
	}
	case TokenType::DoubleLiteral:
	{
		std::string value = m_tokenStream->value;
		eat(TokenType::DoubleLiteral);
		return std::make_shared<LiteralExprAST>(minus+value, TokenType::DoubleLiteral);
	}
	case TokenType::TrueLiteral:
	{
		std::string value = m_tokenStream->value;
		eat(TokenType::TrueLiteral);
		return std::make_shared<LiteralExprAST>(value, TokenType::TrueLiteral);
	}
	case TokenType::FalseLiteral:
	{
		std::string value = m_tokenStream->value;
		eat(TokenType::FalseLiteral);
		return std::make_shared<LiteralExprAST>(value, TokenType::FalseLiteral);
	}
	case TokenType::StringLiteral:
	{
		std::string value = m_tokenStream->value;

		//Remove quotes
		value = value.substr(1, value.size() - 2);
		eat(TokenType::StringLiteral);

		return std::make_shared<LiteralExprAST>(value, TokenType::StringLiteral);
	}
	}
	return nullptr;
}

std::shared_ptr<VarExprAST> Parser::parseVariable()
{
	std::string varName = check({TokenType::Identifier}).value;
	eat(TokenType::Identifier);
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
	return parseAssign();
}
std::shared_ptr<ASTNode> Parser::parseAssign()
{
	std::shared_ptr<ASTNode> lhs = parseLogicalOrExpr();

	while (m_tokenStream->type == TokenType::Assign)
	{
		TokenType op = m_tokenStream->type;
		eat(op);

		std::shared_ptr<ASTNode> rhs = parseLogicalOrExpr();
		lhs = std::make_shared<BinaryExprAST>(op, lhs, rhs);
	}

	return lhs;
}
std::shared_ptr<ASTNode> Parser::parseLogicalOrExpr()
{
	std::shared_ptr<ASTNode> lhs = parseLogicalAndExpr();

	while (m_tokenStream->type == TokenType::Or)
	{
		TokenType op = m_tokenStream->type;
		eat(op);

		std::shared_ptr<ASTNode> rhs = parseLogicalAndExpr();
		lhs = std::make_shared<BinaryExprAST>(op, lhs, rhs);
	}

	return lhs;
}
std::shared_ptr<ASTNode> Parser::parseLogicalAndExpr()
{
	std::shared_ptr<ASTNode> lhs = parseBitOrExpr();

	while (m_tokenStream->type == TokenType::And)
	{
		TokenType op = m_tokenStream->type;
		eat(op);

		std::shared_ptr<ASTNode> rhs = parseBitOrExpr();
		lhs = std::make_shared<BinaryExprAST>(op, lhs, rhs);
	}

	return lhs;
}
std::shared_ptr<ASTNode> Parser::parseBitOrExpr()
{
	std::shared_ptr<ASTNode> lhs = parseBitAndExpr();

	while (m_tokenStream->type == TokenType::BitOr)
	{
		TokenType op = m_tokenStream->type;
		eat(op);

		std::shared_ptr<ASTNode> rhs = parseBitAndExpr();
		lhs = std::make_shared<BinaryExprAST>(op, lhs, rhs);
	}

	return lhs;
}
std::shared_ptr<ASTNode> Parser::parseBitAndExpr()
{
	std::shared_ptr<ASTNode> lhs = parseCompareExpr();

	while (m_tokenStream->type == TokenType::BitAnd)
	{
		TokenType op = m_tokenStream->type;
		eat(op);

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
		eat(op);
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
		eat(op);

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
		eat(op);

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
		eat(op);

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
		else if (m_tokenStream.next().type == TokenType::OpenBrace)
		{
			return parseIndex();
		}
		else if (checkUnary().type != TokenType::Invalid)
		{
			return parseUnary(false);
		}
		return parseVariable();
	}
	else if (checkUnary().type != TokenType::Invalid)
	{
		return parseUnary(true);
	}
	else if (checkLiteral().type != TokenType::Invalid || m_tokenStream->type == TokenType::Minus)
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

std::vector<std::shared_ptr<ASTNode>> Parser::parseStatement()
{
	switch (m_tokenStream->type)
	{
	case TokenType::Def:
		parseFunction();
		break;
	case TokenType::Int:
	case TokenType::Double:
	case TokenType::Bool:
	case TokenType::String:
	case TokenType::Var:
		return parseVarDecl();
	case TokenType::Identifier:
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
	case TokenType::While:
		return { parseWhile() };
	case TokenType::Break:
		return { parseBreak() };
	case TokenType::Continue:
		return { parseContinue() };
	case TokenType::BlockStart:
		return { parseBlock() };
	default:
		return { parseExpression() };
	}
	return {};
}

std::shared_ptr<ASTNode> Parser::parsePrint()
{
	eat(TokenType::Print);

	eat(TokenType::OpenParen);
	std::shared_ptr<ConsoleOutputExprAST> outAST = std::make_shared<ConsoleOutputExprAST>(parseExpression());
	eat(TokenType::CloseParen);

	return outAST;
}

std::vector<std::shared_ptr<ASTNode>> Parser::parseVarDecl()
{
	std::vector<std::shared_ptr<ASTNode>> vars;
	std::vector<std::pair<std::string, std::shared_ptr<ASTNode>>> varsAssign;

	TokenType varType = checkType().type;
	eat(varType);

	//Checking whether the array is
	std::vector<std::shared_ptr<ASTNode>> sizeArray;
	if (check({ TokenType::OpenBrace }).type != TokenType::Invalid)
	{
		while (m_tokenStream->type == TokenType::OpenBrace)
		{
			eat(TokenType::OpenBrace);
			sizeArray.push_back(parseExpression());
			eat(TokenType::CloseBrace);
		}
	}
	bool isReference = false;
	if (check({ TokenType::BitAnd }).type != TokenType::Invalid)
	{
		eat(TokenType::BitAnd);
		isReference = true;
	}
	while (m_tokenStream->type != TokenType::Semicolon)
	{
		if (m_tokenStream->type == TokenType::Comma)
		{
			eat(TokenType::Comma);
			continue;
		}
		else if (m_tokenStream->type == TokenType::Identifier && m_tokenStream.next().type == TokenType::Assign)
		{
			std::string varName = m_tokenStream->value;
			eat(TokenType::Identifier);
			eat(TokenType::Assign);
			std::shared_ptr<ASTNode> value = parseExpression();
			varsAssign.push_back({ varName, value });
		}
		else if(m_tokenStream->type == TokenType::Identifier && m_tokenStream.next().type != TokenType::Assign)
		{
			std::string varName = m_tokenStream->value;
			eat(TokenType::Identifier);
			varsAssign.push_back({ varName, nullptr });
		}
	}
	for (const auto& assign : varsAssign)
	{
		vars.push_back(std::make_shared<VarDeclAST>(assign.first, varType, assign.second, sizeArray, isReference));
	}
	return vars;
}

std::shared_ptr<BlockAST> Parser::parseBlock()
{
	auto prevSymbolTable = SymbolTableManager::getInstance().getSymbolTable();
	auto symbolTableBlock = std::make_shared<SymbolTable>();
	symbolTableBlock->extend(prevSymbolTable.get());
	SymbolTableManager::getInstance().setSymbolTable(symbolTableBlock);

	eat(TokenType::BlockStart);

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
			// There should be no semicolon after these statements
			std::shared_ptr<IfAST> ifAST = std::dynamic_pointer_cast<IfAST>(statements[0]);
			if (ifAST)
			{
				continue;
			}
			std::shared_ptr<WhileAST> whileAST = std::dynamic_pointer_cast<WhileAST>(statements[0]);
			if (whileAST)
			{
				continue;
			}
			std::shared_ptr<BlockAST> blockAST = std::dynamic_pointer_cast<BlockAST>(statements[0]);
			if (blockAST)
			{
				continue;
			}
		}
		eat(TokenType::Semicolon);
	}
	
	eat(TokenType::BlockStop);

	SymbolTableManager::getInstance().setSymbolTable(prevSymbolTable);

	return block;
}

std::vector<std::pair<TokenType, std::string>> Parser::parseArgs()
{
	auto symbolTableFunc = SymbolTableManager::getInstance().getSymbolTable();
	std::vector<std::pair<TokenType, std::string>> args;
	eat(TokenType::OpenParen);
	while(m_tokenStream->type != TokenType::CloseParen)
	{
		TokenType type = checkType().type;
		eat(type);
		std::string name = m_tokenStream->value;
		eat(TokenType::Identifier);
		args.push_back({ type, name });
		symbolTableFunc->addVarType(name, getType(type));
		if (m_tokenStream->type != TokenType::CloseParen)
		{
			eat(TokenType::Comma);
		}
	}
	eat(TokenType::CloseParen);
	return args;
	
}

std::shared_ptr<CallExprAST> Parser::parseCallFunc()
{
	std::string nameFuncCall = m_tokenStream->value;
	eat(TokenType::Identifier);
	std::vector<std::shared_ptr<ASTNode>> callArgs;
	eat(TokenType::OpenParen);
	while(m_tokenStream->type != TokenType::CloseParen)
	{
		if (m_tokenStream->type == TokenType::Comma)
		{
			eat(TokenType::Comma);
			continue;
		}
		std::shared_ptr<ASTNode> arg = parseExpression();
		callArgs.push_back(std::move(arg));
	}
	eat(TokenType::CloseParen);
	return std::make_shared<CallExprAST>(nameFuncCall, std::move(callArgs));
}

std::shared_ptr<ASTNode> Parser::parseFunction()
{
	auto prevSymbolTable = SymbolTableManager::getInstance().getSymbolTable();

	auto symbolTableFunc = std::make_shared<SymbolTable>();
	symbolTableFunc->extend(prevSymbolTable.get());
	SymbolTableManager::getInstance().setSymbolTable(symbolTableFunc);

	eat(TokenType::Def);

	TokenType retType = TokenType::NonType;
	if (checkType().type != TokenType::Invalid)
	{
		retType = checkType().type;
		eat(retType);
	}

	std::string funcName = m_tokenStream->value;
	eat(TokenType::Identifier);
	
	std::vector<std::pair<TokenType, std::string>> args = std::move(parseArgs());

	//Parse the prototype if there is no block
	if (m_tokenStream->type == TokenType::Semicolon)
	{
		eat(TokenType::Semicolon);
		auto proto = std::make_shared<ProtFunctionAST>(funcName, retType, args);
		proto->codegen();
		SymbolTableManager::getInstance().setSymbolTable(prevSymbolTable);
		prevSymbolTable->addFunctionReturnType(funcName, proto->llvmType);
		return proto;
	}

	std::shared_ptr<BlockAST> body = parseBlock();
	auto func = std::make_shared<FunctionAST>(funcName, retType, args, std::move(body));

	func->doSemantic();
	func->codegen();

	SymbolTableManager::getInstance().setSymbolTable(prevSymbolTable);
	prevSymbolTable->addFunctionReturnType(funcName, func->llvmType);
	return func;

}

std::shared_ptr<ReturnAST> Parser::parseReturn()
{
	eat(TokenType::Return);
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

std::shared_ptr<WhileAST> Parser::parseWhile()
{
	eat(TokenType::While);
	std::shared_ptr<ASTNode> whileExpr = parseExpression();
	eat(TokenType::Colon);
	std::shared_ptr<BlockAST> whileBlock = parseBlock();
	return std::make_shared<WhileAST>(whileExpr, whileBlock);
}

std::shared_ptr<BreakAST> Parser::parseBreak()
{
	eat(TokenType::Break);
	return std::make_shared<BreakAST>();
}

std::shared_ptr<ContinueAST> Parser::parseContinue()
{
	eat(TokenType::Continue);
	return std::make_shared<ContinueAST>();
}

std::shared_ptr<IndexExprAST> Parser::parseIndex()
{
	TokenType op;
	std::string varName;
	varName = m_tokenStream->value;
	eat(TokenType::Identifier);
	op = m_tokenStream->type;
	std::vector<std::shared_ptr<ASTNode>> indexes;

	if (op == TokenType::OpenBrace)
	{
		eat(TokenType::OpenBrace);
		indexes.push_back(parseExpression());
		eat(TokenType::CloseBrace);

		while (m_tokenStream->type == TokenType::OpenBrace)
		{
			eat(TokenType::OpenBrace);
			indexes.push_back(parseExpression());
			eat(TokenType::CloseBrace);
		}
	}
	bool getPtr = m_tokenStream->type == TokenType::Assign;
	return std::make_shared<IndexExprAST>(varName, indexes, getPtr);
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
	return check({ TokenType::Int, TokenType::Double, TokenType::Bool, TokenType::String, TokenType::Var});
}

Token Parser::checkLiteral()
{
	return check({ TokenType::IntLiteral, TokenType::DoubleLiteral, TokenType::TrueLiteral, TokenType::FalseLiteral, TokenType::StringLiteral});
}

Token Parser::checkUnary()
{
	if (m_tokenStream.next().type == TokenType::Increment || m_tokenStream.next().type == TokenType::Decrement)
	{
		return m_tokenStream.next();
	}
	return check({TokenType::Decrement, TokenType::Increment});
}

void Parser::eat(TokenType type)
{
	if (m_tokenStream->type != type)
	{
		throw std::runtime_error("ERROR::PARSER::Unknown Token: " + g_nameTypes[static_cast<int>(m_tokenStream->type)] + "\nExpected: " + g_nameTypes[static_cast<int>(type)]);
	}
	m_tokenStream++;
}