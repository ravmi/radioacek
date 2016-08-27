#include "gtest/gtest.h"
#include <iostream>
#include <radioacek/tcp_connection.h>
#include <thread>
#include <radioacek/UDPConnection.h>

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
    UDPConnection client;
    client.connect("localhost", port);
    client.sendMessage("un");
    client.sendMessage("deux");
    client.sendMessage("trois");
    EXPECT_EQ(client.receiveMessage(), "quatre");
    EXPECT_EQ(client.receiveMessage(), "cinq");
    EXPECT_EQ(client.receiveMessage(), "six");
}
TEST(TCPTest, SingleClientCommunication) {
    UDPConnection server;
    int server_port = 12012;
    server.serve(server_port);
    std::thread french_thread(speak_french, server_port);
    EXPECT_EQ(server.receiveMessage(), "un");
    EXPECT_EQ(server.receiveMessage(), "deux");
    EXPECT_EQ(server.receiveMessage(), "trois");

    server.sendMessage("quatre");
    server.sendMessage("cinq");
    server.sendMessage("six");

    french_thread.join();
}
void say_the_word_and_client_port(std::string word, int server_port) {
    std::cout <<"hehet"<<std::endl;
    UDPConnection client;
    client.connect("localhost", server_port);
    client.sendMessage(word + std::to_string(client.server_port()));
    std::cout <<"hehet"<<std::endl;
    sleep(1);
}
TEST(TCPTest, MultipleClientsCommunication) {
    UDPConnection server;
    int server_port = 12013;
    server.serve(server_port);
    std::thread uno_thread(say_the_word_and_client_port, "uno", server_port);
    std::cout <<"hehe"<<std::endl;
    std::string message(server.receiveMessage());
    EXPECT_EQ(message, "uno" + std::to_string(server.last_client_port()));
    std::cout <<"hehe"<<std::endl;
    std::thread dos_thread(say_the_word_and_client_port, "dos", server_port);
    EXPECT_EQ(server.receiveMessage(), "dos" + std::to_string(server.last_client_port()));
    std::thread tres_thread(say_the_word_and_client_port, "tres", server_port);
    EXPECT_EQ(server.receiveMessage(), "tres" + std::to_string(server.last_client_port()));
    uno_thread.join();
    dos_thread.join();
    tres_thread.join();
}
