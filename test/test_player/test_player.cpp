#include "test_player.h"
#include <iostream>

//using ::testing::Return;

TEST(FooTest, ByDefaultBazTrueIsTrue) {
    std::cout << "hehe"<<std::endl;
    std::cerr << "hehe"<<std::endl;
    EXPECT_EQ(false, true);
}

TEST(FooTest, ByDefaultBazFalseIsFalse) {
    EXPECT_EQ(true, true);
}

TEST(FooTest, SometimesBazFalseIsTrue) {
    EXPECT_EQ(true, true);
}
