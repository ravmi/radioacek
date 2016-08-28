/**
 *    Wrapper for UNIX TCP connection.
 *    Whether connects to a server
 *    or waits for connections as a server using TCPMediator
 *    (both functions are blocking).
 *    Then communicates by receiving and sending messages
 */

#ifndef RADIO_ACEK_TCP_CONNECTION
#define RADIO_ACEK_TCP_CONNECTION

#include <string>
namespace radioacek {
class TCPMediator {
    private:
        int socket_ = -1;
        uint16_t port_ = 0;
        static const unsigned int QUEUE_LENGTH = 10;

    public:
        TCPMediator(uint16_t port = 0 /* default random free port */ );

        uint16_t get_port() {
            return port_;
        }

        int get_socket() {
            return socket_;
        }

};
class TCPConnection {
    private:
        static const unsigned int BUFFER_SIZE = 131072;

        int socket_ = -1;
        /* +1 for \0 in c string */
        char buffer_[BUFFER_SIZE + 1];
        size_t buffer_free_index = 0;

    public:
        TCPConnection() = default;

        int get_socket() {
            return socket_;
        }

        size_t size() {
            return buffer_free_index;
        }

        /* all five below are blocking */
        void connect(std::string host_name, const char *c_port);
        TCPConnection(TCPMediator &listener);
        ssize_t sendMessage(std::string message);
        ssize_t sendMessage(const char *message, size_t length);
        /* size = 0 is actually full buffer, shouldn't be used */
        size_t receiveMessage(size_t size = 0);

        char last_char();
        char pop_char();
        /*  Doesn't copy the buffer, may be dangerous
         *  (modified by the next receiveMessage) */
        char* cFlush();
        std::string stringFlush();
};
}

#endif
