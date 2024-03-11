#pragma once

#include "token_type.hpp"

#include <iostream>
#include <string>
#include <optional>

class Token
{
private:
    TokenType m_type;
    std::string m_value;
public:
    Token(TokenType type, std::string value);

    TokenType get_type() const;
    std::string get_value() const;

    void display() const;
};