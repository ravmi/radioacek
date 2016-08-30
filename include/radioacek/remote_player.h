#ifndef RADIOACEK_REMOTEPLAYER_H
#define RADIOACEK_REMOTEPLAYER_H

#include <radioacek/udp_connection.h>
#include <radioacek/exceptions.h>
#include <radioacek/tcp_connection.h>
#include <radioacek/timer.h>

#include <thread>
#include <string>
#include <atomic>
#include <boost/regex.hpp>
#include <time.h>
#include <mutex>

namespace radioacek {
class RemotePlayer {
    private:
        UDPConnection manager;
        const double TITLE_TIMEOUT = 3.0;

    public:
        /* To send answers, mostly exit codes */
        std::shared_ptr<TCPConnection> telnet;

        Timer to_start;
        Timer to_die;

        std::atomic<bool> lives;
        std::mutex open_player;
        bool opened = false;

        /* QUIT before even started? */
        bool canceled = false;

        /* negative values means the thread will live forever */
        const int time_to_die = -1;
        int id;
        std::string server_name;
        int server_port;
        std::string launch_command;
        int exit_code;


    public:
        RemotePlayer(
                std::string server,
                int port,
                std::string command,
                int id,
                std::shared_ptr<TCPConnection> telnet,
                int hour = 0,
                int minutes = 0, 
                int timetodie = -1);

        std::string play();

        std::string title();

        void quit();

        std::string pause();

        int get_socket() {
            return manager.get_socket();
        }

        bool is_alive() {
            return this->lives.load();

        }

        bool is_suicidal() {
            return time_to_die >= 0;
        }

        void connect() {
            manager.connect(server_name, server_port);
        }

        void cancel() {
            canceled = true;
        }

        bool is_canceled() {
            return canceled;
        }

        static void run_player(RemotePlayer *t) {
            using namespace std;
            try {
                t->open_player.lock();
                t->opened = true;
                if(t->canceled) {
                    return;
                    t->lives.store(false);
                }

                t->connect();
                t->telnet->sendMessage(string("OK ") + to_string(t->id) + "\r\n");

                t->to_die.reset();
                int a = system(t->launch_command.c_str());
                t->exit_code = WEXITSTATUS(a);
                if (t->exit_code == 0) {
                    t->telnet->sendMessage(string("OK ") + std::to_string(t->id) + " closed with 0 exit code\r\n");

                }
                else {
                    t->telnet->sendMessage(
                            string("OK ") + std::to_string(t->id) + string(" closed with exit code: " + std::to_string(t->exit_code)) +
                            "\r\n");


                }
                t->lives.store(false);
            }
            catch (ServerClosed &e) {
                t->telnet->mock();
            }
            catch (std::exception &e) {
                try {
                    t->telnet->sendMessage(string("ERROR: player ") + to_string(t->id) + " " + e.what() + string("\r\n"));
                    t->lives.store(false);
                    return;
                }
                catch (std::exception &e) {
                    t->lives.store(false);
                    return;
                }
            }
        }

};
}

#endif
