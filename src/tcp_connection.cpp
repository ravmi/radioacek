#include<string.h>
#include<unistd.h>
#include<netdb.h>
#include<string>

#include <radioacek/tcp_connection.h>
#include <radioacek/exceptions.h>

using std::string;
using std::to_string;
namespace radioacek {
TCPMediator::TCPMediator(uint16_t port) {
    /* IPv4, TCP */
    this->socket_ = ::socket(PF_INET, SOCK_STREAM, 0);
    if (socket_ < 0)
        throw (ConnectionError("Socket init error\n"));

    struct sockaddr_in server_address;
    server_address.sin_family = PF_INET;
    /* listening on all interfaces */
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(port);
    if (bind(
                socket_,
                (struct sockaddr *) &server_address,
                sizeof(server_address)) < 0) {
        throw (ConnectionError("Bind error"));
    }
    if (listen(socket_, QUEUE_LENGTH) < 0) {
        throw (ConnectionError("Listen error"));
    }
    socklen_t addrlen = sizeof(server_address);
    if (getsockname(
                socket_,
                (struct sockaddr *) &server_address,
                &addrlen) != 0) {
        throw ConnectionError("Couldn't get server info");
    }
    port_ = ntohs(server_address.sin_port);
}
void TCPConnection::connect(string host_name, const char *c_port) {

    struct addrinfo addr_hints;
    struct addrinfo *addr_result;
    memset(&addr_hints, 0, sizeof(struct addrinfo));

    addr_hints.ai_family = AF_INET; // IPv4
    addr_hints.ai_socktype = SOCK_STREAM;
    addr_hints.ai_protocol = IPPROTO_TCP;
    if (getaddrinfo(host_name.c_str(), c_port, &addr_hints, &addr_result))
        throw ConnectionError(("Wrong host info"));

    socket_ = ::socket(
            addr_result->ai_family,
            addr_result->ai_socktype,
            addr_result->ai_protocol);
    if (socket_ < 0)
        throw (ConnectionError("Socket init error"));

    if (::connect(socket_, addr_result->ai_addr, addr_result->ai_addrlen) < 0)
        throw (ConnectionError("Couldn't find host"));
    freeaddrinfo(addr_result);
}
/*
   void TCPConnection::disconnect() {
   if (close(socket_) != 0)
   throw (ConnectionError("Disconnect Error"));
   socket_ = -1;
   }
   */

TCPConnection::TCPConnection(TCPMediator &listener) {
    struct sockaddr_in client_address;

    socklen_t client_address_len = sizeof(client_address);
    int msg_sock = ::accept(
            listener.get_socket(),
            (struct sockaddr *) &client_address,
            &client_address_len);
    if (msg_sock < 0)
        throw (ConnectionError("Couldn't accept"));
    this->socket_ = msg_sock;
    buffer_free_index = 0;
}

ssize_t TCPConnection::sendMessage(string message) {
    size_t length = message.size();
    const char *c_message = message.c_str();
    ssize_t ret = write(socket_, c_message, length);
    if (ret < 0)
        throw ConnectionError(strerror(errno));
    return ret;

}

ssize_t TCPConnection::sendMessage(const char *message, size_t length) {
    int written = write(socket_, message, length);
    if(written < 0)
        throw ConnectionError(strerror(errno));
    return written;
}

size_t TCPConnection::receiveMessage(size_t size) {
    if (size == 0)
        size = BUFFER_SIZE - buffer_free_index;
    if (size > (BUFFER_SIZE - buffer_free_index))
        throw BufferOverflowException(this->BUFFER_SIZE - buffer_free_index, size);
    int r = (size_t) read(socket_, buffer_ + buffer_free_index, size);
    if (r == 0)
        throw (ServerClosedError("while receiving a message"));
    if (r < 0)
        throw (ConnectionError(strerror(errno)));
    else {
        buffer_free_index += r;
        return r;
    }
}

char TCPConnection::last_char() {
    if (buffer_free_index == 0)
        throw (ConnectionError("Nothing yet received"));
    return buffer_[buffer_free_index - 1];
}

char TCPConnection::pop_char() {
    if(buffer_free_index == 0)
        throw ConnectionError("Nothing yet received");
    return buffer_[--buffer_free_index];
}

char* TCPConnection::cFlush() {
    buffer_[buffer_free_index] = '\0';
    buffer_free_index = 0;
    return buffer_;
}
string TCPConnection::stringFlush() {
    size_t length = buffer_free_index;
    buffer_free_index = 0;
    return string(buffer_, length);
}
}
