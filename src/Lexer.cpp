#include "Lexer.h"

Lexer::Lexer(const std::string& input) : m_input(input), m_currentPos(0)
{

}
Token Lexer::getNextToken() {
    skipWhitespaceAndComments();

    if (m_currentPos >= m_input.length()) {
        return Token{ TokenType::EndOfFile, "" };
    }

    char current_char = m_input[m_currentPos];

    if (isalpha(current_char)) {
        return getIdentifier();
    }
    else if (isdigit(current_char)) {
        return getNumber();
    }
    else if (isOperator(current_char)) {
        return getOperator();
    }
    else if (isPunctuation(current_char)) {
        return getPunctuation();
    }
    else if (isQuote(current_char)) {
        return getStringLiteral();
    }
    else {
        throw std::runtime_error("ERROR::Lexer::Invalid character: " + current_char);
    }
}
Token Lexer::getIdentifier() {
    size_t startPos = m_currentPos;

    while (m_currentPos < m_input.length() && isalnum(m_input[m_currentPos])) {
        m_currentPos++;
    }
    std::string identifier = m_input.substr(startPos, m_currentPos - startPos);
    if (identifier == "if")
    {
        return Token{ TokenType::If,identifier };
    }
    else if (identifier == "else")
    {
        return Token{ TokenType::Else,identifier };
    }
    else if (identifier == "while")
    {
        return Token{ TokenType::While,identifier };
    }
    else if (identifier == "int")
    {
        return Token{ TokenType::Int, identifier };
    }
    else if (identifier == "double")
    {
        return Token{ TokenType::Double, identifier };
    }
    else if (identifier == "bool")
    {
        return Token{ TokenType::Bool, identifier };
    }
    else if (identifier == "true")
    {
        return Token{ TokenType::TrueLiteral, identifier };
    }
    else if (identifier == "false")
    {
        return Token{ TokenType::FalseLiteral, identifier };
    }
    else if (identifier == "def")
    {
        return Token{ TokenType::Def,identifier };
    }
    else if (identifier == "return")
    {
        return Token{ TokenType::Return, identifier };
    }
    else if (identifier == "print")
    {
        return Token{ TokenType::Print, identifier };
    }
    return Token{ TokenType::Identifier, identifier };
}
Token Lexer::getNumber() {
    size_t startPos = m_currentPos;
    bool isFloat = false;
    while (m_currentPos < m_input.length() && (isdigit(m_input[m_currentPos]) || (m_input[m_currentPos] == '.' && m_currentPos != startPos))) {
        if (m_input[m_currentPos] == '.')
        {
            isFloat = true;
        }
        m_currentPos++;
    }
    if (isFloat)
    {
        return Token{ TokenType::DoubleLiteral, m_input.substr(startPos, m_currentPos - startPos) };
    }
    return Token{ TokenType::IntLiteral, m_input.substr(startPos, m_currentPos - startPos) };
}
Token Lexer::getOperator() {
    size_t startPos = m_currentPos;

    while (m_currentPos < m_input.length() && isOperator(m_input[m_currentPos])) {
        m_currentPos++;
    }
    std::string op = m_input.substr(startPos, m_currentPos - startPos);
    if (op == "+")
    {
        return Token{ TokenType::Plus, op };
    }
    else if (op == "++")
    {
        return Token{ TokenType::Increment, op };
    }
    else if (op == "-")
    {
        return Token{ TokenType::Minus, op };
    }
    else if (op == "--")
    {
        return Token{ TokenType::Decrement, op };
    }
    else if (op == "*")
    {
        return Token{ TokenType::Mul, op };
    }
    else if (op == "/")
    {
        return Token{ TokenType::Div, op };
    }
    else if (op == "=")
    {
        return Token{ TokenType::Assign, op };
    }
    else if (op == "==")
    {
        return Token{ TokenType::Equal, op };
    }
    else if (op == ">")
    {
        return Token{ TokenType::Greater, op };
    }
    else if (op == "<")
    {
        return Token{ TokenType::Less, op };
    }
    else if (op == "&")
    {
        return Token{ TokenType::BitAnd, op };
    }
    else if (op == "|")
    {
        return Token{ TokenType::BitOr, op };
    }
    else if (op == "&&")
    {
        return Token{ TokenType::And, op };
    }
    else if (op == "||")
    {
        return Token{ TokenType::Or, op };
    }
    else if (op == "**")
    {
        return Token{ TokenType::Exponentiation, op };
    }
    return Token{ TokenType::Invalid, op };
}
Token Lexer::getPunctuation() {
    size_t startPos = m_currentPos;
    m_currentPos++;
    if (m_input[m_currentPos - 1] == ';')
    {
        return Token{ TokenType::Semicolon, m_input.substr(startPos, m_currentPos - startPos) };
    }
    else if (m_input[m_currentPos - 1] == ':')
    {
        return Token{ TokenType::Colon, m_input.substr(startPos, m_currentPos - startPos) };
    }
    else if (m_input[m_currentPos - 1] == '(')
    {
        return Token{ TokenType::OpenParen, m_input.substr(startPos, m_currentPos - startPos) };
    }
    else if (m_input[m_currentPos - 1] == ')')
    {
        return Token{ TokenType::CloseParen, m_input.substr(startPos, m_currentPos - startPos) };
    }
    else if (m_input[m_currentPos - 1] == ',')
    {
        return Token{ TokenType::Comma, m_input.substr(startPos, m_currentPos - startPos) };
    }
    else if (m_input[m_currentPos - 1] == '{')
    {
        return Token{ TokenType::BlockStart, m_input.substr(startPos, m_currentPos - startPos) };
    }
    else if (m_input[m_currentPos - 1] == '}')
    {
        return Token{ TokenType::BlockStop, m_input.substr(startPos, m_currentPos - startPos) };
    }
    return Token{ TokenType::Invalid, m_input.substr(startPos, m_currentPos - startPos) };
}
Token Lexer::getStringLiteral() {
    size_t startPos = m_currentPos;
    char quoteChar = m_input[m_currentPos];
    m_currentPos++;

    while (m_currentPos < m_input.length() && m_input[m_currentPos] != quoteChar) {
        m_currentPos++;
    }

    if (m_currentPos >= m_input.length()) {
        throw std::runtime_error("ERROR::Lexer::Missing closing quote for string literal");
    }

    m_currentPos++;
    return Token{ TokenType::StringLiteral, m_input.substr(startPos, m_currentPos - startPos) };
}
bool Lexer::isOperator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '=' || c == '<' || c == '>' || c == '&' || c =='|';
}
bool Lexer::isPunctuation(char c) {
    return c == ';' || c == ',' || c == '(' || c == ')' || c == '{' || c == '}' || c == ':';
}
bool Lexer::isQuote(char c) {
    return c == '\'' || c == '"';
}
void Lexer::skipWhitespaceAndComments() {
    while (m_currentPos < m_input.length() && (isspace(m_input[m_currentPos]) || isComment())) {
        if (isComment()) {
            skipComment();
        }
        else {
            m_currentPos++;
        }
    }
}
bool Lexer::isComment() {
    return m_input[m_currentPos] == '/' && m_currentPos + 1 < m_input.length() && m_input[m_currentPos + 1] == '/';
}
void Lexer::skipComment() {
    while (m_currentPos < m_input.length() && m_input[m_currentPos] != '\n') {
        m_currentPos++;
    }
}