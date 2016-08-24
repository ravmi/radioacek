#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stddef.h>
#include "Connection.h"
#include "ConnectionError.h"
#include "BufferOverflowException.h"
#include "ServerClosedError.h"

#ifndef SIECI2_UDPCONNECTION_H
#define SIECI2_UDPCONNECTION_H

/* exception we should remember: ServerClosedError, ConnectionError */
class UDPConnection : public Connection {
    static const unsigned int BUFFER_SIZE = 8192;
    int flags;
    int server_flags;
public:
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;


    UDPConnection() : Connection() {
       flags = 0;
       server_flags = 0;


       // after socket() call; we should close(sock) on any execution path;
       // since all execution paths exit immediately, sock would be closed when program terminates
    }

    /* Sending message after serving isn't a good idea - you have to receive something to get the client */
    void serve(uint16_t listen_port) {
       nsocket = socket(PF_INET, SOCK_DGRAM, 0); // creating IPv4 UDP socket
       if (nsocket < 0)
          throw ConnectionError("Socket error");
       server_address.sin_family = AF_INET; // IPv4
       server_address.sin_addr.s_addr = htonl(INADDR_ANY); // listening on all interfaces
       server_address.sin_port = htons(listen_port); // default port for receiving is PORT_NUM

       // bind the socket to a concrete address

       int bind_ret = bind(nsocket, (struct sockaddr *) &server_address, (socklen_t) sizeof(server_address));
       if (bind_ret < 0)
          throw ConnectionError("Bind error");

    }

    /* after closing you can serve again */
    void close_server() {
       if (close(nsocket))
          throw (ConnectionError(strerror(errno)));
       nsocket = socket(AF_INET, SOCK_DGRAM, 0); // creating IPv4 UDP socket
       if (nsocket < 0)
          throw ConnectionError("Socket error");

    }

    /* all it does is just finding a place to send */
    void connect(string server_name, uint16_t server_port) {
       nsocket = socket(PF_INET, SOCK_DGRAM, 0); // creating IPv4 UDP socket
       if (nsocket < 0)
          throw ConnectionError("Socket error");
       struct addrinfo addr_hints;
       struct addrinfo *addr_result;

       (void) memset(&addr_hints, 0, sizeof(struct addrinfo));
       addr_hints.ai_family = AF_INET; // IPv4
       addr_hints.ai_socktype = SOCK_DGRAM;
       addr_hints.ai_protocol = IPPROTO_UDP;
       addr_hints.ai_flags = 0;
       addr_hints.ai_addrlen = 0;
       addr_hints.ai_addr = NULL;
       addr_hints.ai_canonname = NULL;
       addr_hints.ai_next = NULL;
       int err = getaddrinfo(server_name.c_str(), NULL, &addr_hints, &addr_result);
       if (err != 0)
          throw (ConnectionError(string(strerror(errno))));
       client_address.sin_family = AF_INET; // IPv4
       client_address.sin_addr.s_addr =
               ((struct sockaddr_in *) (addr_result->ai_addr))->sin_addr.s_addr; // address IP
       client_address.sin_port = htons(server_port);
       freeaddrinfo(addr_result);


    }


    string receiveMessage(unsigned int length = BUFFER_SIZE, bool replace_last_client = true) {
       char buffer[BUFFER_SIZE];

       struct sockaddr_in new_client_address;

       socklen_t client_address_size = (socklen_t) sizeof(new_client_address);
       ssize_t receive_len = recvfrom(nsocket, buffer, sizeof(buffer), flags, (struct sockaddr *) &new_client_address,
                                      &client_address_size);
       if (receive_len < 0)
          throw (ConnectionError(strerror(errno)));
       else if (receive_len == 0)
          throw ServerClosedError("closed when receiving a message");
       if (replace_last_client)
          client_address = new_client_address;
       return string(buffer, (unsigned int) receive_len);
    }

    ssize_t sendMessage(string message) {
       socklen_t client_address_size = (socklen_t) sizeof(client_address);

       unsigned long length = message.length();
       if (length > BUFFER_SIZE)
          throw (BufferOverflowException(BUFFER_SIZE, length));
       char buffer[BUFFER_SIZE];
       memcpy(buffer, message.c_str(), length);
       ssize_t written = sendto(nsocket, buffer, (size_t) length, server_flags, (struct sockaddr *) &client_address,
                                client_address_size);
       if (written < 0)
          throw (ConnectionError(strerror(errno)));
       return written;
    }


};

#endif //SIECI2_UDPCONNECTION_H
