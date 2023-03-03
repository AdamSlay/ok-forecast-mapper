#include <gtest/gtest.h>

//#include "../src/main.cpp"

// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions) {
// Expect two strings not to be equal.
EXPECT_STRNE("hello", "world");
// Expect equality.
EXPECT_EQ(7 * 6, 42);
}

//TEST(MainTest, getX_zero) {
//    EXPECT_EQ(getX(-180), 10);
//}