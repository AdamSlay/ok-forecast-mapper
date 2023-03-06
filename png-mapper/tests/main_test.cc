#include <gtest/gtest.h>

#include "../src/utils.h"

TEST(UtilsTest, getX_zero) {
    EXPECT_EQ(getX(0), 6670);
}

TEST(UtilsTest, getX_neg_100) {
EXPECT_EQ(getX(-100), 270);
}

TEST(UtilsTest, getX_pos_100) {
EXPECT_EQ(getX(100), 13070);
}

TEST(UtilsTest, getX_neg_180) {
EXPECT_EQ(getX(-180), -4850);
}

TEST(UtilsTest, getY_zero) {
EXPECT_EQ(getY(0), 2680);
}

TEST(UtilsTest, getY_pos_30) {
EXPECT_EQ(getY(30), 666);
}

TEST(UtilsTest, getY_neg_30) {
EXPECT_EQ(getY(-30), 4694);
}

TEST(UtilsTest, map_value_zero) {
EXPECT_EQ(map_value(0, 0, 100), 0);
}

TEST(UtilsTest, map_value_50) {
EXPECT_EQ(map_value(50, 0, 100), 127);
}

TEST(UtilsTest, map_value_100) {
EXPECT_EQ(map_value(100, 0, 100), 255);
}
