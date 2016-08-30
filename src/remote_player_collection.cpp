#include <radioacek/remote_player.h>
#include <radioacek/remote_player_collection.h>
#include <vector>
#include <boost/regex.hpp>

using namespace std;
namespace radioacek {
bool RemotePlayerCollection::parse_start(
        const string to_parse,
        string& host,
        string& arguments,
        int& port) {
    boost::smatch result;
    if (boost::regex_match(to_parse, result, boost::regex(start_regex))) {
        host = result[1];
        arguments = result[2];
        string filename = result[3];
        if(filename == string("-"))
            throw ParseException("Can't use cout remotely");
        port = std::stoi(result[4]);
        return true;
    }
    else {
        return false;
    }
}
bool RemotePlayerCollection::parse_ppqt(
        const string to_parse,
        string& command_name,
        int& id) {
    boost::smatch result;
    if (boost::regex_match(to_parse, result, boost::regex(play_pause_quit_title_regex))) {
        command_name = result[1];
        id = std::stoi(result[2]);
        return true;
    }
    else {
        return false;
    }
}

bool RemotePlayerCollection::parse_at(
        const string to_parse,
        int& hour,
        int& minutes,
        int& time_to_die,
        string& host,
        string& arguments,
        int& port) {
    boost::smatch result;
    if (boost::regex_match(to_parse, result, boost::regex(at_regex))) {
        hour = std::stoi(result[1]);
        minutes = std::stoi(result[2]);
        time_to_die = std::stoi(result[3]);
        host = result[4];
        arguments = result[5];
        port = std::stoi(result[7]);
        return true;
    }
    else {
        return false;
    }
}

/* Cleans inactive players */
int RemotePlayerCollection::insert(
        string server,
        int port,
        string command,
        std::shared_ptr<TCPConnection> telnet,
        int hour,
        int minutes,
        int time_to_die) {
    while ((unsigned int)it != rplayers.size()) {

        if (!rplayers[it]->is_alive()) {
            rplayers[it] = std::make_shared<RemotePlayer>(
                    server,
                    port,
                    command,
                    it,
                    telnet,
                    hour,
                    minutes,
                    time_to_die);
            break;
        }
        else ++it;
    }
    int ret = it;
    if ((unsigned int)it == rplayers.size()) {
        rplayers.push_back((std::make_shared<RemotePlayer>(
                        server,
                        port,
                        command,
                        it,
                        telnet,
                        hour,
                        minutes,
                        time_to_die)));
        it = 0;
    }
    return ret;

}


bool RemotePlayerCollection::active(unsigned int i) {
    if(i < rplayers.size())
        return rplayers[i]->is_alive();
    else {
        return false;
    }
}

std::shared_ptr<RemotePlayer> RemotePlayerCollection::get(unsigned int i) {
    if (i > rplayers.size())
        throw RemotePlayerException(i, "Wrong index");
    return (rplayers[i]);
}

void RemotePlayerCollection::cleanup() {
    for(auto player : rplayers) {
        if(!player->opened && player->to_start.passed()) {
            player->open_player.unlock();
        }
        if(player->is_suicidal() && player->to_die.passed() && !player->is_canceled())
            player->quit();
    }
}
}
