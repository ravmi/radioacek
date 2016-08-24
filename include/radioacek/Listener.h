//
// Created by rm on 30.05.16.
//

#ifndef SIK_POLL_H
#define SIK_POLL_H

#include <poll.h>
#include <vector>
#include <stddef.h>
#include "ConnectionError.h"
#include <string>
#include <string.h>

class Listener {

private:
    struct pollfd *pfd;
    size_t size;
    int last;


public:
    Listener(std::vector<int> vdescriptors) {
       last = 0;
       size = vdescriptors.size();
       pfd = new struct pollfd[size];
       for (int i = 0; i < size; i++) {
          pfd[i].fd = vdescriptors[i];
          pfd[i].events = POLLIN;
          pfd[i].revents = 0;
       }
    }
    Listener(int desc) {
       pfd = new struct pollfd[1];
       pfd[0].fd = desc;
       pfd[0].events = POLLIN;
       pfd[0].revents = 0;
       size = 1;
    }

    /* Socket which received an event or -1 if timeout */
    int listen(int timeout) {

       int retpoll = poll(pfd, size, timeout);
       if (retpoll > 0) {
          for (; last < size; last = (last + 1) % size) {
             if (pfd[last].revents & POLLIN) {
                pfd[last].revents = 0;
                return pfd[last].fd;
             }
          }
       }
       else if (retpoll == 0) {
          return -1;
       }
       else
          throw ConnectionError(string(strerror(errno)));
    }

    ~Listener() {
       delete[] pfd;
    }
};


#endif //SIK_POLL_H
