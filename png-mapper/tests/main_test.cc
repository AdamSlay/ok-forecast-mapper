#include <gtest/gtest.h>

#include "../src/utils.h"

// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions) {
EXPECT_STRNE("hello", "world");
EXPECT_EQ(7 * 6, 42);
}

TEST(MainTest, getX_zero) {
    EXPECT_EQ(getX(-180), -4850);
}