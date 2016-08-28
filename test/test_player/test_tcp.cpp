#include "gtest/gtest.h"
#include <iostream>
#include <thread>
#include <radioacek/tcp_connection.h>
#include <algorithm>

//using ::testing::Return;


// The fixture for testing class Foo.
class TCPTest : public ::testing::Test {

protected:

    // You can do set-up work for each test here.
    TCPTest() {
    }
/**
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
    // */
};
TEST(TCPTest, ServeTest) {
    radioacek::TCPMediator med;
}
