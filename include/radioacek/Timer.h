//
// Created by rm on 31.05.16.
//

#ifndef SIK_TIMER_H
#define SIK_TIMER_H

#include <ctime>

class Timer {
private:
    time_t start_time;
    double delay_time;
public:
    Timer(double delay) {
       time(&start_time);
       delay_time = delay;
    }
    void reset() {
       time(&start_time);
    }

    /* True if delay passed */
    bool check() {
       time_t current;
       time(&current);
       return difftime(current, start_time) > delay_time;
    }

};


#endif //SIK_TIMER_H
