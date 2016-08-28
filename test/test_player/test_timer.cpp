#include "gtest/gtest.h"
#include <iostream>
#include <radioacek/timer.h>

#include <chrono>
#include <thread>

using namespace radioacek;

class TimerTest : public ::testing::Test {};

TEST(TimerTest, HaveElapsedTest) {
    Timer t(1.2);
    std::this_thread::sleep_for(std::chrono::milliseconds(1200));

    EXPECT_TRUE(t.passed());
}

TEST(TimerTest, HaventElapsedTest) {
    Timer t(0.01);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    EXPECT_FALSE(t.passed());
}

TEST(TimerTest, PassedWithVerySmallTimeToPassTest) {
    Timer t(0.0000001);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    EXPECT_TRUE(t.passed());
}
TEST(TimerTest, PassedAssertionTest) {
    Timer t;
    EXPECT_DEATH(t.passed(), "time_to_pass > 0.0");
}
