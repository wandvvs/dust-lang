#pragma once

#include "../token/token.hpp"

#include <vector>

class Lexer
{
private:
    const std::string m_source;
    size_t m_pos = 0;
    char m_current;

    void move_next();

public:
    explicit Lexer(const std::string& source);

    std::vector<Token> tokenize();
};