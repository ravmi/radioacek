#include "gtest/gtest.h"
#include <iostream>
#include <thread>
#include <radioacek/udp_connection.h>
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
void speak_french(int port) {
    radioacek::UDPConnection client;
    client.connect("localhost", port);
    client.sendMessage("un");
    EXPECT_EQ(client.receiveMessage(), "quatre");
    client.sendMessage("deux");
    EXPECT_EQ(client.receiveMessage(), "cinq");
    client.sendMessage("trois");
    EXPECT_EQ(client.receiveMessage(), "six");
}
TEST(TCPTest, SingleClientCommunication) {
    radioacek::UDPConnection server;
    int server_port = 12012;
    server.serve(server_port);
    std::thread french_thread(speak_french, server_port);
    EXPECT_EQ(server.receiveMessage(), "un");
    server.sendMessage("quatre");
    EXPECT_EQ(server.receiveMessage(), "deux");
    server.sendMessage("cinq");
    EXPECT_EQ(server.receiveMessage(), "trois");
    server.sendMessage("six");

    french_thread.join();
}

void say_the_word_and_repeat_with_your_port(std::string word, int server_port) {
    radioacek::UDPConnection client;
    client.connect("localhost", server_port);
    client.sendMessage(word);
    EXPECT_EQ(client.receiveMessage(), word + std::to_string(client.my_port()));
}

TEST(TCPTest, MultipleClientsCommunication) {
    radioacek::UDPConnection server;
    int server_port = 12013;
    server.serve(server_port);

    std::thread uno_thread(say_the_word_and_repeat_with_your_port, "uno", server_port);
    EXPECT_EQ(server.receiveMessage(), "uno");
    server.sendMessage("uno" + std::to_string(server.speaker_port()));
    int first_speaker_port = server.speaker_port();


    std::thread dos_thread(say_the_word_and_repeat_with_your_port, "dos", server_port);
    EXPECT_EQ(server.receiveMessage(), "dos");
    server.sendMessage("dos" + std::to_string(server.speaker_port()));

    std::thread tres_thread(say_the_word_and_repeat_with_your_port, "tres", server_port);
    EXPECT_EQ(server.receiveMessage(), "tres");
    server.sendMessage("tres" + std::to_string(server.speaker_port()));

    uno_thread.join();
    dos_thread.join();
    tres_thread.join();
}


