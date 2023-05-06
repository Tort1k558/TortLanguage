#include "Parser.h"
#include<stdexcept>
#include<memory>
#include<iostream>
using namespace llvm;
Parser::Parser(TokenStream *stream) :
	m_src(stream),
	m_currentPos(m_src->begin())
{
	parse();
}
void Parser::parse()
{
	while (m_currentPos != TokenType::EndOfFile)
	{
		switch (m_currentPos.type())
		{
		case TokenType::Def:
			m_currentPos++;
			parseFunction();
			break;
		case TokenType::Semicolon:
			m_currentPos++;
			break;
		default:
			m_currentPos++;
			break;
		}
	}
}
void Parser::parseFunction()
{
	check(TokenType::Identifier);
} 
void Parser::check(TokenType type)
{
	if (m_currentPos != type)
	{
		throw std::runtime_error(g_nameTypes[static_cast<int>((&m_currentPos).type)] + "!=" + g_nameTypes[static_cast<int>(type)]);
	}
	m_currentPos++;
}
