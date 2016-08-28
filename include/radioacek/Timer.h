#ifndef RADIOACEK_TIMER_H
#define RADIOACEK_TIMER_H

#include <ctime>

class Timer {
private:
    time_t start_time;
    double delay_time;
public:
    Timer() = delete;
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
