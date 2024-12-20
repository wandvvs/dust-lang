#include "lexer.hpp"

#include <cctype>
#include <cstdlib>
#include <stdexcept>
#include <vector>

Lexer::Lexer(std::string source)
    : m_source(source), m_pos(0)
{
    if (!m_source.empty())
        m_current = m_source[0];
}

void Lexer::move_next()
{
    ++m_pos;

    if (m_pos > m_source.size() - 1)
        m_current = '\0';
    else
        m_current = m_source[m_pos];
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
                else if (keyword == "mut")
                {
                    tokens.emplace_back(TokenType::MUT, std::move(keyword));
                }
                else if (keyword == "const")
                {
                    tokens.emplace_back(TokenType::CONST, std::move(keyword));
                }
                else if (keyword == "true")
                {
                    tokens.emplace_back(TokenType::TRUE, std::move(keyword));
                }
                else if (keyword == "false")
                {
                    tokens.emplace_back(TokenType::FALSE, std::move(keyword));
                }
                else if (keyword == "use") {
                move_next();
                std::string next_keyword;
                while (std::isalpha(m_current)) {
                    next_keyword.push_back(m_current);
                    move_next();
                }
                if (next_keyword == "io") {
                    tokens.emplace_back(TokenType::USE_IO, "use io");
                } else {
                    tokens.emplace_back(TokenType::IDENTIFIER, std::move(keyword));
                }
            }
                else
                {
                    tokens.emplace_back(TokenType::IDENTIFIER, std::move(keyword));
                }
            }
            else if (std::isdigit(m_current) || m_current == '.')
            {
                std::string initial_digit;
                bool hasDecimal = false;

                while(std::isdigit(m_current) || (!hasDecimal && m_current == '.'))
                {
                    if (m_current == '.')
                    {
                        hasDecimal = true;
                    }
                    initial_digit.push_back(m_current);
                    move_next();
                }

                if (hasDecimal)
                {
                    tokens.emplace_back(TokenType::FLOAT_LITERAL, std::move(initial_digit));
                }
                else
                {
                    tokens.emplace_back(TokenType::INT_LITERAL, std::move(initial_digit));
                }
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
            else if(m_current == '<')
            {
                tokens.emplace_back(TokenType::LESS, "<");
                move_next();
            }
            else if(m_current == '>')
            {
                tokens.emplace_back(TokenType::MORE, ">");
                move_next();
            }
            else if(m_current == '?')
            {
                tokens.emplace_back(TokenType::CHECK, "?");
                move_next();
            }
            else if(m_current == ')')
            {
                tokens.emplace_back(TokenType::RPAREN, ")");
                move_next();
            }

            else if(m_current == '=')
            {
                move_next();
                if(m_current == '=')
                {
                    tokens.emplace_back(TokenType::EQUAL, "==");
                    move_next();
                }
                else
                {
                    tokens.emplace_back(TokenType::ASSIGN, "=");
                }
            }

            else if(m_current == '!')
            {
                move_next();
                if(m_current == '=')
                {
                    tokens.emplace_back(TokenType::NOT_EQUAL, "!=");
                    move_next();
                }
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
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            move_next();
        }
    }

    return tokens;
}
