#pragma once
#include<string>
#include<vector>
enum class TokenType {
    EndOfFile = 0,
    Identifier,
    Number,
    Plus,
    Minus,
    Mul,
    Div,
    Exponentiation,
    Equal,
    Less,
    Greater,
    Not,
    And,
    Or,
    BitAnd,
    BitOr,
    Comma,
    Dot,
    Assign,
    Condition,
    While,
    If,
    Else,
    Def,
    Bool,
    Int,
    Double,
    Void,
    NonType,
    IntNumber,
    DoubleNumber,
    CharLiteral,
    StringLiteral,
    Return,
    OpenParen,
    CloseParen,
    BlockStart,
    BlockStop,
    Semicolon,
    Print,
    Invalid
};
const std::vector<std::string> g_nameTypes = {
    "EndOfFile",
    "Identifier",
    "Number",
    "Plus",
    "Minus",
    "Mul",
    "Div",
    "Exponentiation",
    "Equal",
    "Less",
    "Greater",
    "Not",
    "And",
    "Or",
    "BitAnd",
    "BitOr",
    "Comma",
    "Dot",
    "Assign",
    "Condition",
    "While",
    "If",
    "Else",
    "Def",
    "Bool",
    "Int",
    "Double",
    "Void",
    "NonType",
    "IntNumber",
    "DoubleNumber",
    "CharLiteral",
    "StringLiteral",
    "Return",
    "OpenParen",
    "CloseParen",
    "BlockStart",
    "BlockStop",
    "Semicolon",
    "Print",
    "Invalid"
};
struct Token {
    TokenType type;
    std::string value;
};