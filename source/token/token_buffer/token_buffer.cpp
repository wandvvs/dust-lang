#include "token_buffer.hpp"

std::vector<Token>::const_iterator TokenBuffer::begin() const { return m_tokens.begin(); }
std::vector<Token>::const_iterator TokenBuffer::end() const { return m_tokens.end(); }

TokenBuffer::TokenBuffer(std::vector<Token> tokens)
    : m_tokens(std::move(tokens)), m_current(m_tokens.at(0)) {}

void TokenBuffer::move_next()
{
    ++pos;

    if(!(pos >= m_tokens.size()))
    {
        m_current = m_tokens.at(pos);
    }
}