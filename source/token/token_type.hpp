#pragma once

#include <string>
#include <sstream>

enum class TokenType
{
    EXIT,
    INT_LITERAL,
    FLOAT_LITERAL,
    STRING_LITERAL,
    TRUE,
    FALSE,
    LPAREN,
    RPAREN,
    SEMICOLON,
    WRITELN,
    QOUTE,
    MUT,
    CONST,
    ASSIGN,
    IDENTIFIER,
    PLUS,
    MINUS,
    MUL,
    DIV,
    EXTERN_STD,
    CHECK,
    MORE,
    LESS,
    EQUAL,
    NOT_EQUAL
};

inline std::string token_type_to_string(TokenType m_type)
{
    switch (m_type)
    {
        case TokenType::EXIT:
            return "EXIT";
        case TokenType::CHECK:
            return "CHECK";
        case TokenType::MORE:
            return "MORE";
        case TokenType::NOT_EQUAL:
            return "NOT EQUAL";
        case TokenType::LESS:
            return "LESS";
        case TokenType::EQUAL:
            return "EQUAL";
        case TokenType::CONST:
            return "CONST";
        case TokenType::TRUE:
            return "TRUE";
        case TokenType::FALSE:
            return "FALSE";
        case TokenType::INT_LITERAL:
            return "INT_LITERAL";
        case TokenType::EXTERN_STD:
            return "EXTERN_STD";
        case TokenType::PLUS:
            return "PLUS";
        case TokenType::MINUS:
            return "MINUS";
        case TokenType::DIV:
            return "DIV";
        case TokenType::MUL:
            return "MUL";
        case TokenType::LPAREN:
            return "LPAREN";
        case TokenType::RPAREN:
            return "RPAREN";
        case TokenType::SEMICOLON:
            return "SEMICOLON";
        case TokenType::WRITELN:
            return "WRITELN";
        case TokenType::STRING_LITERAL:
            return "STRING_LITERAL";
        case TokenType::QOUTE:
            return "QOUTE";
        case TokenType::MUT:
            return "MUT";
        case TokenType::ASSIGN:
            return "ASSIGN";
        case TokenType::IDENTIFIER:
            return "IDENTIFIER";
        case TokenType::FLOAT_LITERAL:
            return "FLOAT_LITERAL";
        default:
            return "UNKNOWN";
    }
}