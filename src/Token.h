#pragma once
#include<string>
#include<vector>
enum class TokenType {
    EndOfFile = 0,
    Identifier,
    Number,
    Increment,
    Decrement,
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
    IntLiteral,
    TrueLiteral,
    FalseLiteral,
    DoubleLiteral,
    CharLiteral,
    StringLiteral,
    Return,
    OpenParen,
    CloseParen,
    BlockStart,
    BlockStop,
    Semicolon,
    Colon,
    Print,
    Invalid
};
const std::vector<std::string> g_nameTypes = {
    "EndOfFile",
    "Identifier",
    "Number",
    "Increment",
    "Decrement",
    "Plus",
    "Minus",
    "Mul",
    "Div",
    "Exponentiation",
    "Equal",
    "Less",
    "Greater",
    "Not",
    "LogicalAnd",
    "LogicalOr",
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
    "IntLiteral",
    "TrueLiteral",
    "FalseLiteral",
    "DoubleLiteral",
    "CharLiteral",
    "StringLiteral",
    "Return",
    "OpenParen",
    "CloseParen",
    "BlockStart",
    "BlockStop",
    "Semicolon",
    "Colon",
    "Print",
    "Invalid"
};
struct Token {
    TokenType type;
    std::string value;
};