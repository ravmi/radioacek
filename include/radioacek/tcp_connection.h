/**
 *    Wrapper for UNIX TCP connection.
 *    Whether waits for incoming connection or connects to a server
 *    (both functions are blocking).
 *    Then communicates by receiving and sending messages
 */

#ifndef RADIO_ACEK_TCP_CONNECTION
#define RADIO_ACEK_TCP_CONNECTION

#include <sys/types.h>
#include<stdlib.h> /* strtol */
#include<string.h> /* strlen */
#include<limits.h> /* UINT_MAX */
#include<stdio.h>
#include<stdint.h> /*uint16_t */
#include<poll.h>
#include<sys/socket.h>
#include<unistd.h> /* write */
#include"err.h"
#include<stdbool.h>
#include<netdb.h>
#include<signal.h>
#include "err.h"
#include "Connection.h"
#include "BufferOverflowException.h"
#include "ConnectionError.h"
#include "ServerClosedError.h"
#include<string>
#include <string.h>
#include<iostream>
#include <cerrno>





using std::string;
using std::to_string;
/* exception we should remember: ServerClosedError, ConnectionError */
class TCPConnection : public Connection {

public:
    static const unsigned int BUFFER_SIZE = 131072;
    static const unsigned int QUEUE_LENGTH = 10;


protected:
    char buffer[BUFFER_SIZE + 1];
    size_t end = 0;

public:
    TCPConnection() = default;


    void connect(string host_name, const char *c_port) {
       const char *c_host_name = host_name.c_str();

       struct addrinfo addr_hints;
       struct addrinfo *addr_result;

       // 'converting' host/port in string to struct addrinfo
       memset(&addr_hints, 0, sizeof(struct addrinfo));
       addr_hints.ai_family = AF_INET; // IPv4
       addr_hints.ai_socktype = SOCK_STREAM;
       addr_hints.ai_protocol = IPPROTO_TCP;
       if (getaddrinfo(c_host_name, c_port, &addr_hints, &addr_result))
          throw ConnectionError(("Wrong host info"));
       // initialize socket according to getaddrinfo results
       nsocket = socket(addr_result->ai_family, addr_result->ai_socktype, addr_result->ai_protocol);
       if (nsocket < 0)
          throw (ConnectionError("Socket init error"));
       // connect socket to the server
       if (::connect(nsocket, addr_result->ai_addr, addr_result->ai_addrlen) < 0)
          throw (ConnectionError("Couldn't find host"));
       freeaddrinfo(addr_result);
    }

    void disconnect() {
       if (close(nsocket) != 0)
          throw (ConnectionError("Disconnect Error"));
       nsocket = -1;
    }

    /* Return port */
    uint16_t serve(uint16_t port) {
       struct sockaddr_in server_address;

       this->nsocket = socket(PF_INET, SOCK_STREAM, 0);
       if (nsocket < 0)
          throw (ConnectionError("Socket init error\n"));
       server_address.sin_family = AF_INET; // IPv4
       server_address.sin_addr.s_addr = htonl(INADDR_ANY); // listening on all interfaces
       server_address.sin_port = htons(port);
       if (bind(nsocket, (struct sockaddr *) &server_address, sizeof(server_address)) < 0)
          throw (ConnectionError("Bind error"));
       if (listen(nsocket, QUEUE_LENGTH) < 0)
          throw (ConnectionError("Listen error"));
       socklen_t addrlen = sizeof(server_address);
       if (getsockname(nsocket, (struct sockaddr *) &server_address, &addrlen) != 0)
          throw ConnectionError("Couldn't get server info");
       return ntohs(server_address.sin_port);

    }

    TCPConnection(TCPConnection &listener) {
       struct sockaddr_in client_address;

       socklen_t client_address_len = sizeof(client_address);
       int msg_sock = ::accept(listener.get_socket(), (struct sockaddr *) &client_address, &client_address_len);
       if (msg_sock < 0)
          throw (ConnectionError("Couldn't accept"));
       this->nsocket = msg_sock;
       end = 0;

    }

    ssize_t sendMessage(string message) {
       size_t length = message.size();
       const char *c_message = message.c_str();
       ssize_t ret = write(nsocket, c_message, length);
       if (ret < 0)
          throw ConnectionError(strerror(errno));
       return ret;

    }

    ssize_t sendMessage(const char *message, size_t length) {
        int written = write(nsocket, message, length);
        if(written < 0)
            throw ConnectionError(strerror(errno));
        return written;
    }

    size_t receiveMessage(size_t size = 0) {
       if (size == 0)
          size = BUFFER_SIZE - end;
       if (size > (BUFFER_SIZE - end))
          throw BufferOverflowException(this->BUFFER_SIZE - end, size);
       int r = (size_t) read(nsocket, buffer + end, size);
       if (r == 0)
          throw (ServerClosedError("while receiving a message"));
       if (r < 0)
          throw (ConnectionError(strerror(errno)));
       else {
          end += r;
          return r;
       }
    }


    char last() {
       if (end == 0)
          throw (ConnectionError("Nothing yet received"));
       return buffer[end - 1];
    }
    char back() {
       if(end == 0)
          throw ConnectionError("Nothing yet received");
       return buffer[--end];
    }

    size_t size() {
       return end;
    }

    char* flush() {
       end = 0;
       return buffer;
    }
    string stringFlush() {
       size_t length = end;
       end = 0;
       return string(buffer, length);
    }
};


#endif
