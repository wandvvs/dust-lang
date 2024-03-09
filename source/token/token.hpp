#pragma once

#include "token_type.hpp"

#include <iostream>
#include <string>
#include <optional>

class Token
{
private:
    TokenType m_type;
    std::optional<std::string> m_value;
public:
    Token(TokenType type, std::optional<std::string> value);

    TokenType get_type() const;
    std::optional<std::string> get_value() const;

    void display() const;
};