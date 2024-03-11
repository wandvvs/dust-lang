#pragma once

#include <string>

enum class TokenType
{
    EXIT,
    INT_LITERAL,
    STRING_LITERAL,
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
    FUNC,
    RETURN_TYPE,
    LEFT_BRACE,
    RIGHT_BRACE,
    EXTERN_STD,
};

constexpr inline std::string token_type_to_string(TokenType m_type)
{
    switch (m_type)
    {
        case TokenType::EXIT:
            return "EXIT";
        case TokenType::CONST:
            return "CONST";
        case TokenType::INT_LITERAL:
            return "INT_LITERAL";
        case TokenType::FUNC:
            return "FUNC";
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
        case TokenType::RETURN_TYPE:
            return "RETURN_TYPE";
        case TokenType::LEFT_BRACE:
            return "LEFT_BRACE";
        case TokenType::RIGHT_BRACE:
            return "RIGHT_BRACE";
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
        default:
            return "UNKNOWN";
    }
}