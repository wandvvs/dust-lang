#include <gtest/gtest.h>
#include <vector>

#include "../source/lexer/lexer.hpp"
#include "../source/token/token_buffer/token_buffer.hpp"
#include "../source/compiler/llvm_compiler.hpp"

TEST(LexerTest, TokenizeTest)
{
    const std::string source = "mut a = ? 5+2 == 10.5;";

    Lexer lexer(std::move(source));
    std::vector<Token> tokens = lexer.tokenize();

    ASSERT_EQ(tokens.size(), 10);

    EXPECT_EQ(tokens[0].get_type(), TokenType::MUT);
    EXPECT_EQ(tokens[1].get_type(), TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[2].get_type(), TokenType::ASSIGN);
    EXPECT_EQ(tokens[3].get_type(), TokenType::CHECK);
    EXPECT_EQ(tokens[4].get_type(), TokenType::INT_LITERAL);
    EXPECT_EQ(tokens[4].get_value(), "5");
    EXPECT_EQ(tokens[5].get_type(), TokenType::PLUS);
    EXPECT_EQ(tokens[6].get_type(), TokenType::INT_LITERAL);
    EXPECT_EQ(tokens[6].get_value(), "2");
    EXPECT_EQ(tokens[7].get_type(), TokenType::EQUAL);
    EXPECT_EQ(tokens[8].get_type(), TokenType::FLOAT_LITERAL);
    EXPECT_EQ(tokens[8].get_value(), "10.5");
    EXPECT_EQ(tokens[9].get_type(), TokenType::SEMICOLON);
}

TEST(LexerTest, LexerSourcePassing)
{
    const std::string source = "mut a = 5;";

    Lexer lexer(source);

    const std::vector<Token> tokens = lexer.tokenize();

    EXPECT_EQ(source, lexer.get_source());
}

TEST(TokenBufferTest, VectorPassTest)
{
    std::string source = "mut a;";

    Lexer lexer(std::move(source));

    ASSERT_TRUE(source.empty());
    ASSERT_EQ(source.size(), 0);

    std::vector<Token> tokens = lexer.tokenize();

    TokenBuffer buffer(std::move(tokens));

    EXPECT_EQ(buffer.m_current.get_type(), TokenType::MUT);
    EXPECT_EQ(buffer.m_current.get_value(), "mut");

    buffer.move_next();

    EXPECT_EQ(buffer.m_current.get_type(), TokenType::IDENTIFIER);

    ASSERT_EQ(buffer.m_tokens.size(), 3);

    ASSERT_EQ(tokens.size(), 0);
    ASSERT_TRUE(tokens.empty());
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}