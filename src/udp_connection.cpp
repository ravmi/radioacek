#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <cassert>

#include <radioacek/exceptions.h>
#include <radioacek/udp_connection.h>
using namespace std;
namespace radioacek {

void UDPConnection::serve(uint16_t listen_port) {
    assert(!connected_ && !serving_);
    /* IPv4 UDP socket */
    socket_ = ::socket(PF_INET, SOCK_DGRAM, 0);
    if (socket_ < 0)
        throw ConnectionError("Socket error");

    /* IPv4 */
    my_address_.sin_family = AF_INET;
    /* listening on all interfaces */
    my_address_.sin_addr.s_addr = htonl(INADDR_ANY);
    my_address_.sin_port = htons(listen_port);

    /* bind the socket to a concrete address */
    int bind_ret = bind(
            socket_,
            (struct sockaddr *) &my_address_,
            (socklen_t) sizeof(my_address_));
    if (bind_ret < 0)
        throw ConnectionError("Bind error");

	struct sockaddr_in new_my_address;
	socklen_t new_my_address_length = sizeof(new_my_address);
	if (getsockname(
				socket_,
				(struct sockaddr *)& new_my_address,
				&new_my_address_length) == -1)
		throw (ConnectionError(strerror(errno)));
	my_address_ = new_my_address;
    serving_ = true;
}
/**
void UDPConnection::close_server() {
    if (close(socket_))
        throw (ConnectionError(strerror(errno)));
    socket_ = ::socket(AF_INET, SOCK_DGRAM, 0); // creating IPv4 UDP socket
    if (socket_ < 0)
        throw ConnectionError("Socket error");
    serving_ = true;
}
*/

void UDPConnection::connect(const std::string server_name, const uint16_t server_port) {
    assert(!connected_ && !serving_);
    /* IPv4 UDP socket */
    socket_ = ::socket(PF_INET, SOCK_DGRAM, 0);
    if (socket_ < 0)
        throw ConnectionError("Socket error");

    struct addrinfo addr_hints;
    struct addrinfo *addr_result;

    (void) memset(&addr_hints, 0, sizeof(struct addrinfo));
    /* IPv4 */
    addr_hints.ai_family = AF_INET;
    /* UDP */
    addr_hints.ai_socktype = SOCK_DGRAM;
    addr_hints.ai_protocol = IPPROTO_UDP;
    addr_hints.ai_flags = 0;
    addr_hints.ai_addrlen = 0;
    addr_hints.ai_addr = NULL;
    addr_hints.ai_canonname = NULL;
    addr_hints.ai_next = NULL;
    if(getaddrinfo(
                server_name.c_str(),
                NULL,
                &addr_hints,
                &addr_result) != 0)
        throw (ConnectionError(std::string(strerror(errno))));
    /* IPv4 */
    speaker_address_.sin_family = AF_INET;
    /* IP address */
    speaker_address_.sin_addr.s_addr =
        ((sockaddr_in *) (addr_result->ai_addr))->sin_addr.s_addr;
    speaker_address_.sin_port = htons(server_port);
    freeaddrinfo(addr_result);
    connected_ = true;
}


std::string UDPConnection::receiveMessage(unsigned int limit) {
    assert(connected_ || serving_);
    char buffer[limit];

    struct sockaddr_in new_speaker_address;
    socklen_t new_speaker_address_size = (socklen_t) sizeof(new_speaker_address);
    ssize_t receive_len = recvfrom(
            socket_,
            buffer,
            sizeof(buffer),
            flags_,
            (struct sockaddr *) &new_speaker_address,
            &new_speaker_address_size);
    if (receive_len < 0)
        throw (ConnectionError(strerror(errno)));
    else if (receive_len == 0)
        throw ServerClosed("Closed when receiving a message");


    speaker_address_ = new_speaker_address;
    connected_ = true;
    return string(buffer, (unsigned int) receive_len);
}

ssize_t UDPConnection::sendMessage(string message) {
    assert(connected_);
    socklen_t speaker_address_size = (socklen_t) sizeof(speaker_address_);

    unsigned long length = message.length();
    if (length > BUFFER_SIZE)
        throw (BufferOverflowException(BUFFER_SIZE, length));
    char buffer[BUFFER_SIZE];
    memcpy(buffer, message.c_str(), length);
    ssize_t written = sendto(
            socket_,
            buffer,
            (size_t) length,
            server_flags_,
            (struct sockaddr *) &speaker_address_,
            speaker_address_size);
	if (written < 0)
		throw (ConnectionError(strerror(errno)));

	struct sockaddr_in new_my_address;
	socklen_t new_my_address_length = sizeof(new_my_address);
	if (getsockname(
				socket_,
				(struct sockaddr *)& new_my_address,
				&new_my_address_length) == -1)
		throw (ConnectionError(strerror(errno)));
	my_address_ = new_my_address;

	return written;
}
}
