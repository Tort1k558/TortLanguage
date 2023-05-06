#pragma once
#include"Token.h"
#include<stdexcept>
class Lexer {
public:
    Lexer(std::string input);

    Token getNextToken();

private:
    Token getIdentifier();
    Token getNumber();
    Token getOperator();
    Token getPunctuation();
    Token getStringLiteral();
    bool isOperator(char c);
    bool isPunctuation(char c);
    bool isQuote(char c);
    void skipWhitespaceAndComments();
    bool isComment();
    void skipComment();

    const std::string m_input;
    size_t m_currentPos;

};
