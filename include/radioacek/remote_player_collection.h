#ifndef RADIOACEK_REMOTE_COLLECTION
#define RADIOACEK_REMOTE_COLLECTION

#include <radioacek/remote_player.h>
#include <vector>
#include <boost/regex.h>

namespace radioacek {

class RemotePlayerCollection {
    private:
        std::vector<std::shared_ptr<RemotePlayer> > rplayers;
        const std::string start_regex = 
            "START (\\S+) (.* (\\S+) (\\d{1,5}) \\S+)";
        const std::string at_regex = 
            "AT (\\d{1,2})[.](\\d{1,2}) (\\d{1,9}+) (\\S+) (.* (\\S+) (\\d{1,5}) \\S+)";
        const std::string play_pause_quit_title_regex =
            "(?:(PAUSE|PLAY|QUIT|TITLE) (\\d{1,5}))";

        int it;

    public:

        RemotePlayerCollection() {
            it = 0;
        }

        bool parse_start(
                const std::string to_parse,
                std::string& host,
                std::string& arguments,
                int& port);

        bool parse_ppqt(
                const std::string to_parse,
                std::string& command_name,
                int& id);

        bool parse_at(
                const std::string to_parse,
                int& hour,
                int& minutes,
                int& time_to_die,
                std::string& host,
                std::string& arguments,
                int& port);


        /* also cleans inactive players */
        int insert(
                std::string server,
                int port,
                std::string command,
                std::shared_ptr<TCPConnection> telnet,
                int hour = 0,
                int minutes = 0,
                int time_to_die = -1);

        bool active(unsigned int index);

        std::shared_ptr<RemotePlayer> get(unsigned int index);

        /* manages remote players waiting for execution and timeout */
        void cleanup();
};
}
#endif
