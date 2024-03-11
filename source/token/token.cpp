#include "token.hpp"
#include <optional>

Token::Token(TokenType type, std::string value)
    : m_type(type), m_value(std::move(value)) {}

TokenType Token::get_type() const { return m_type; }

std::string Token::get_value() const { return m_value; }

void Token::display() const
{
    std::cout << "Type: " << token_type_to_string(m_type) << "\t";
    std::cout << "Value: " << m_value << std::endl;
}