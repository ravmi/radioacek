/**
 *    @author Rafal Michaluk
 *    @brief  Listener - wrapper around UNIX poll function
 */

#ifndef SIK_LISTENER
#define SIK_LISTENER

#include <vector>
#include <poll.h>

namespace radioacek {

class Listener {

    private:
        std::vector<pollfd> ears;
        int next_to_listen = 0;

        bool hears(const pollfd& ear) {
            return ear.revents & POLLIN;
        }

        void forget(pollfd& ear) {
            ear.revents = 0;
        }

    public:
        Listener() = delete;
        Listener(const std::vector<int>& descriptors);

        /* Returns the index of socket that received an event or -1 if timeout */
        int listen(int timeout = -1/* default infinite timeout */);
};
}
#endif
