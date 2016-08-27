#include "gtest/gtest.h"
#include <iostream>
#include <radioacek/listener.h>

//using ::testing::Return;


// The fixture for testing class Foo.
class FooTest : public ::testing::Test {

protected:

    // You can do set-up work for each test here.
    FooTest();

    // You can do clean-up work that doesn't throw exceptions here.
    virtual ~FooTest();

    // If the constructor and destructor are not enough for setting up
    // and cleaning up each test, you can define the following methods:

    // Code here will be called immediately after the constructor (right
    // before each test).
    virtual void SetUp();

    // Code here will be called immediately after each test (right
    // before the destructor).
    virtual void TearDown();

    // The mock bar library shaed by all tests
};
TEST(FooTest, testListener) {
    using namespace radioacek;
    int a = 1;
std::vector<int> b;
    b.push_back(a);
    Listener lis(b);
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
