#include "gtest/gtest.h"
#include <iostream>
#include <thread>
#include <radioacek/tcp_connection.h>
#include <algorithm>
#include <exception>
#include <mutex>

//using ::testing::Return;
using namespace radioacek;
using namespace std;

std::mutex server_started;
const unsigned long int seed = 1472352785;
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
void server_speaking_english(uint16_t port) {
    TCPMediator med(port);

    TCPConnection tcp(med);
    tcp.receiveMessage(12);
    EXPECT_EQ(tcp.stringFlush(), "cat");

    TCPConnection tcp2(med);
    tcp2.receiveMessage(12);
    EXPECT_EQ(tcp2.stringFlush(), "dog");

    TCPConnection tcp3(med);
    tcp3.receiveMessage(12);
    EXPECT_EQ(tcp3.stringFlush(), "bird");
}

TEST(TCPTest, MultipleClientsTest) {
    int port = rand() % 60000 + 1024;
    std::thread english_thread(server_speaking_english, port);
    english_thread.detach();
    sleep(1);

    TCPConnection tcp;
    tcp.connect("localhost", port);
    tcp.sendMessage("cat");

    TCPConnection tcp2;
    tcp2.connect("localhost", port);
    tcp2.sendMessage("dog");

    TCPConnection tcp3;
    tcp3.connect("localhost", port);
    tcp3.sendMessage("bird");
}

void server_spamming(uint16_t port) {
    TCPMediator med(port);

    TCPConnection tcp(med);
    tcp.receive_message_blocking(21);
    EXPECT_EQ(tcp.stringFlush(), "abcdefghijklmnoprstuw");

    tcp.sendMessage("a");
    tcp.sendMessage("bcd");
    tcp.sendMessage("ef");
    tcp.sendMessage("gh");
    tcp.sendMessage("ijk");
    tcp.sendMessage("lm");
    tcp.sendMessage("no");
    tcp.sendMessage("prs");
    tcp.sendMessage("tu");
    tcp.sendMessage("w");
}

TEST(TCPTest, SpammingServerTest) {
    
    int port = rand() % 60000 + 1024;
    std::thread english_thread(server_spamming, port);
    english_thread.detach();
    sleep(1);

    TCPConnection tcp;
    tcp.connect("localhost", port);
    tcp.sendMessage("a");
    tcp.sendMessage("bcd");
    tcp.sendMessage("ef");
    tcp.sendMessage("gh");
    tcp.sendMessage("ijk");
    tcp.sendMessage("lm");
    tcp.sendMessage("no");
    tcp.sendMessage("prs");
    tcp.sendMessage("tu");
    tcp.sendMessage("w");

    tcp.receive_message_blocking(21);
    EXPECT_EQ(tcp.stringFlush(), "abcdefghijklmnoprstuw");
}
