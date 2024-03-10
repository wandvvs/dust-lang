#include "lexer.hpp"

#include <cctype>
#include <cstdlib>
#include <stdexcept>

Lexer::Lexer(const std::string& source)
    : m_source(source), m_pos(0)
{
    if (!m_source.empty())
        m_current = m_source[0];
}

void Lexer::move_next()
{
    ++m_pos;

    if (m_pos > m_source.size() - 1)
    {
        m_current = '\0';
    }
    else
    {
        m_current = m_source[m_pos];
    }
}

std::vector<Token> Lexer::tokenize()
{
    std::vector<Token> tokens;

    while (m_current != '\0')
    {
        if (!std::isspace(m_current))
        {
            if (std::isalpha(m_current))
            {
                std::string keyword;

                while (std::isalpha(m_current))
                {
                    keyword.push_back(m_current);
                    move_next();
                }

                if (keyword == "exit")
                {
                    tokens.emplace_back(TokenType::EXIT, std::move(keyword));
                }
                else if (keyword == "writeln")
                {
                    tokens.emplace_back(TokenType::WRITELN, std::move(keyword));
                }
                else if (keyword == "let")
                {
                    tokens.emplace_back(TokenType::LET, std::move(keyword));
                }
                else if (keyword == "func")
                {
                    tokens.emplace_back(TokenType::FUNC, std::move(keyword));
                }
                else if (keyword == "void")
                {
                    tokens.emplace_back(TokenType::RETURN_TYPE, std::move(keyword));
                }
                else
                {
                    tokens.emplace_back(TokenType::IDENTIFIER, std::move(keyword));
                }
            }

            else if (std::isdigit(m_current))
            {
                std::string initial_digit;

                while(std::isdigit(m_current))
                {
                    initial_digit.push_back(m_current);
                    move_next();
                }

                tokens.emplace_back(TokenType::INT_LITERAL, std::move(initial_digit));
            }

            else if(m_current == ';')
            {
                tokens.emplace_back(TokenType::SEMICOLON, ";");
                move_next();
            }

            else if(m_current == '(')
            {
                tokens.emplace_back(TokenType::LPAREN, "(");
                move_next();
            }

            else if (m_current == '+')
            {
                tokens.emplace_back(TokenType::PLUS, "+");
                move_next();
            }
            else if (m_current == '-')
            {
                tokens.emplace_back(TokenType::MINUS, "-");
                move_next();
            }
            else if (m_current == '*')
            {
                tokens.emplace_back(TokenType::MUL, "*");
                move_next();
            }
            else if (m_current == '/')
            {
                tokens.emplace_back(TokenType::DIV, "/");
                move_next();
            }

            else if(m_current == ')')
            {
                tokens.emplace_back(TokenType::RPAREN, ")");
                move_next();
            }

            else if (m_current == '{')
            {
                tokens.emplace_back(TokenType::LEFT_BRACE, "{");
                move_next();
            }
            else if (m_current == '}')
            {
                tokens.emplace_back(TokenType::RIGHT_BRACE, "}");
                move_next();
            }

            else if(m_current == '=')
            {
                tokens.emplace_back(TokenType::ASSIGN, "=");
                move_next();
            }

            else if(m_current == '"')
            {
                tokens.emplace_back(TokenType::QOUTE, "\"");
                move_next();

                std::string string_literal = "";

                while(m_current != '\"' && m_current != '\0')
                {
                    string_literal.push_back(m_current);
                    move_next();
                }

                std::cout << m_current << std::endl;
                if(m_current != '\"')
                {
                    std::cerr << "Lexing error. Closing quote not found." << std::endl;
                    exit(EXIT_FAILURE);
                }
                tokens.emplace_back(TokenType::STRING_LITERAL, std::move(string_literal));
                tokens.emplace_back(TokenType::QOUTE, "\"");

                move_next();
            }
            else
            {
                std::cerr << "Unexpected symbol: " << m_current << std::endl;
                throw std::runtime_error("Lexer error. Unexpected error.");
            }
        }
        else
        {
            move_next();
        }
    }

    return tokens;
}
