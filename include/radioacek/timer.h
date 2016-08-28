/**
 * Simple timer used to check if given amount of time have passed
 */
#ifndef RADIOACEK_TIMER_H
#define RADIOACEK_TIMER_H

#include <chrono>
#include <thread>
#include <cassert>
namespace radioacek {

class Timer {

    private:
        std::chrono::time_point<std::chrono::system_clock> start;
        double time_to_pass;

    public:
        Timer(double seconds_to_pass = 0.0) {
            start = std::chrono::system_clock::now();
            this->time_to_pass = seconds_to_pass;
        }

        bool passed() {
            assert(time_to_pass > 0.0);
            auto end = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsed_seconds = end-start;
            return elapsed_seconds.count() >= time_to_pass;
        }

        void reset() {
            start = std::chrono::system_clock::now();
        }

        /* time in seconds from creation or last reset */
        double time() {
            auto end = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsed_seconds = end-start;
            return elapsed_seconds.count();
        }
};
}


#endif
