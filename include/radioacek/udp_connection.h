#ifndef RADIOACEK_UDP_CONNECTION_H
#define RADIOACEK_UDP_CONNECTION_H

#include <netinet/in.h>
#include <string>

namespace radioacek {
/**
 *   Simulates continous connection.
 *   Start with whether connect or serve
 *   Then you can exchange messages with receiveMessage and sendMessage;
 */
class UDPConnection {
    private:
        static const unsigned int BUFFER_SIZE = 8192;
        bool connected_ = false;
        bool serving_ = false;
        int flags_ = 0;
        int server_flags_ = 0;

        int socket_ = -1;
        struct sockaddr_in my_address_;
        struct sockaddr_in speaker_address_;

    public:
        UDPConnection() = default;

        uint16_t my_port() {
            return ntohs(my_address_.sin_port);
        }

        uint16_t speaker_port() {
            return ntohs(speaker_address_.sin_port);
        }

        int get_socket() {
            return socket_;
        }

        void serve(uint16_t listen_port);
        void connect(const std::string server_name, const uint16_t server_port);
        /* receives the message from the last host that contacted you */
        std::string receiveMessage(unsigned int limit = BUFFER_SIZE);
        /* Sends a message to the last host that contacted you */
        ssize_t sendMessage(std::string message);
};
}

#endif
