#include <gtest/gtest.h>

// Soon

TEST(NAME, NAME)
{
    ASSERT_TRUE(2+2==4);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}