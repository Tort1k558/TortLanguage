#pragma once
#include<memory>
#include"Token.h"
#include"Lexer.h"
#include<llvm/Support/SMloc.h>

class TokenStream
{
public:
	TokenStream(std::shared_ptr<Lexer> lexer);

	Token next();
	Token prev();
	Token& operator*();
	Token* operator->();

	TokenStream& operator++();
	TokenStream operator++(int);
	TokenStream& operator--();
	TokenStream operator--(int);
private:
	std::shared_ptr<Lexer> m_lexer;
	std::vector<Token> m_tokens;
	size_t m_currentPos;
};

