#include <radioacek/udp_connection.h>
#include <radioacek/exceptions.h>
#include <radioacek/tcp_connection.h>
#include <radioacek/listener.h>
#include <radioacek/timer.h>
#include <radioacek/remote_player.h>

#include <thread>
#include <regex>
#include <string>
#include <atomic>
#include <boost/regex.hpp>
#include <time.h>
#include <mutex>

using namespace std;
namespace radioacek {

    int how_long_to_end(int end_hour, int end_minutes) {
        time_t current = time(0);
        struct tm *end_tm = localtime(&current);
        end_tm->tm_hour = end_hour;
        end_tm->tm_min = end_minutes;
        end_tm->tm_sec = 0;

        time_t end_t = mktime(end_tm);

        /* add one day */
        if (end_t < current) {
            end_t += 24 * 60 * 60;
        }
        return end_t - current;
    }

    RemotePlayer::RemotePlayer(
            string server,
            int port,
            string command,
            int id,
            std::shared_ptr<TCPConnection> telnet,
            int hour,
            int minutes, 
            int timetodie) :
        to_start(how_long_to_end(hour,minutes)),
        telnet(telnet),
        id(id),
        time_to_die(timetodie),
        to_die(timetodie * 60)
    {
        if(!is_suicidal())
            to_start.reinit(0);

        this->server_name = server;
        this->server_port = port;

        this->lives = true;
        this->open_player.lock();
        this->launch_command = "ssh " + server_name + " \"bash -cl 'player " + command + "'\"";
        std::thread start(run_player, this);
        start.detach();
    }



    string RemotePlayer::play() {
        manager.sendMessage("PLAY");
        return std::string("OK ") + std::to_string(id);
    }

    string RemotePlayer::title() {
        manager.sendMessage("TITLE");
        radioacek::Listener l({manager.get_socket()});

        if (l.listen(TITLE_TIMEOUT) < 0) {
            return "ERROR " + std::to_string(id) + string(" title not received");
        }
        else {
            return manager.receiveMessage();
        }
    }

    void RemotePlayer::quit() {
        cancel();
        manager.sendMessage("QUIT");
    }

    string RemotePlayer::pause() {
        manager.sendMessage("PAUSE");
        return std::string("OK ") + std::to_string(id);
    }
}
