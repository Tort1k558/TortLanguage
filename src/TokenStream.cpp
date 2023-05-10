#include "TokenStream.h"

TokenStream::TokenStream(std::shared_ptr<Lexer> lex)
    : m_lexer(std::move(lex)),
	m_currentPos(0)
{
	Token token = m_lexer->getNextToken();
	while (token.type != TokenType::EndOfFile)
	{
		m_tokens.push_back(token);
		token = m_lexer->getNextToken();
	}
	m_tokens.push_back(token);
}

Token TokenStream::next()
{
	return m_tokens[m_currentPos + 1];
}

Token TokenStream::prev()
{
	return m_tokens[m_currentPos - 1];
}

Token& TokenStream::operator*()
{
	return m_tokens[m_currentPos];
}

Token* TokenStream::operator->()
{
	return &m_tokens[m_currentPos];
}

TokenStream& TokenStream::operator++()
{
	if (m_currentPos < m_tokens.size() - 1)
	{
		++m_currentPos;
	}
	return *this;
}

TokenStream TokenStream::operator++(int)
{
	TokenStream temp(*this);
	++(*this);
	return temp;
}

TokenStream& TokenStream::operator--()
{
	if (m_currentPos > 0)
	{
		--m_currentPos;
	}
	return *this;
}

TokenStream TokenStream::operator--(int)
{
	TokenStream temp(*this);
	--(*this);
	return temp;
}