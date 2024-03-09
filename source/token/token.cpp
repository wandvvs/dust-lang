#include "token.hpp"
#include <optional>

Token::Token(TokenType type, std::optional<std::string> value)
    : m_type(type), m_value(std::move(value)) {}

TokenType Token::get_type() const { return m_type; }

std::optional<std::string> Token::get_value() const { return m_value; }

void Token::display() const
{
    std::cout << "Type: " << token_type_to_string(m_type) << "\t";
    if (m_value.has_value())
    {
        std::cout << "Value: " << m_value.value() << std::endl;
    }
    else
    {
        std::cout << "Value: <no value>" << std::endl;
    }
}