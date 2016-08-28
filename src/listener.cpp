/**
 *    @author Rafal Michaluk
 *    @brief  Listener - wrapper around UNIX poll function
 */

#include <vector>
#include <poll.h>
#include <string>

#include <cstring>
#include <cerrno>
#include <cassert>

#include <radioacek/exceptions.h>
#include <radioacek/listener.h>

namespace radioacek {

Listener::Listener(const std::vector<int>& descriptors) {
    next_to_listen = 0;
    for (const int descriptor_number : descriptors) {
        pollfd ear;
        ear.fd = descriptor_number;
        /* excepting only "There is data to read" event */
        ear.events = POLLIN;
        /* no events received yet */
        ear.revents = 0;
        ears.push_back(ear);
    }
}

int Listener::listen(const double timeout) {
    int noise = poll(ears.data(), ears.size(), (int)(timeout * 1000));
    if (noise > 0) {
        while (next_to_listen < (int)ears.size()) {
            if (hears(ears[next_to_listen])) {
                forget(ears[next_to_listen]);
                assert(next_to_listen < ears.size());
                int active = next_to_listen;
                next_to_listen = (next_to_listen + 1) % ears.size();
                return active;
            }
            next_to_listen = (next_to_listen + 1) % ears.size();
        }
        throw ConnectionError(std::string("Poll error"));
    }
    else if (noise == 0/* timeout */) {
        return -1;
    }
    else {
        throw ConnectionError(std::string(std::strerror(errno)));
    }
}
}
