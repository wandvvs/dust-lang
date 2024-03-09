#pragma once

#include "../token.hpp"

#include <vector>

class TokenBuffer
{
public:
    std::vector<Token> m_tokens;
    std::optional<Token> m_current;
    size_t pos = 0;

public:
    TokenBuffer();
    TokenBuffer(std::vector<Token> tokens);

    void move_next();

    std::vector<Token>::const_iterator begin() const;
    std::vector<Token>::const_iterator end() const;
};