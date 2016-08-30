#include <stdint.h>
#include <iostream>
#include <radioacek/tcp_connection.h>
#include <radioacek/remote_player_collection.h>
#include <radioacek/listener.h>
#include <boost/regex.hpp>

#include <string>
#include <thread>

using std::cerr;
using std::endl;
using std::string;
using std::cout;
using std::thread;
using std::to_string;
using std::thread;
using namespace boost;


const int TELNET_BUFFER = 60000;
const int TELNET_TIMEOUT = 1;
namespace radioacek {
uint16_t parse_port(string sport) {
   try {
      int iport = std::stoi(sport);
      if (iport > UINT16_MAX || iport < 0) {
         cerr << "Port too big" << endl;
         exit(1);
      }
      return (uint16_t) iport;

   } catch (std::exception &e) {
      cerr << "Couldn't parse port" << endl;
      exit(1);
   }
}


/* Returns prompt message without trash characters or an empty string if the message was too long */
/* notice: if the message actually was an empy string it may be confused with too long message */
/* blocking */
std::string telnet_receive_message(TCPConnection& telnet) {
    bool too_long = false;
    unsigned char previous;
    unsigned char last = 'x';
    telnet.receiveMessage();
    bool first = true;
    if(telnet.last_char() == '\n')
        telnet.pop_char();
    do {
        if(!first)
            telnet.receiveMessage(1);
        previous = last;
        last = telnet.last_char();
        if(last == 255) {
            telnet.pop_char();
            telnet.receiveMessage(1);
            last = telnet.last_char();
            if(last >= 251 && last <= 254) {
                telnet.pop_char();
                telnet.receiveMessage(1);
                telnet.pop_char();
            }
            else if(last != 255)
                telnet.pop_char();
            last = telnet.last_char();
        }
        if (telnet.size() > TELNET_BUFFER) {
            telnet.cFlush();
            too_long = true;
            break;
        }
        first = false;
        /* accepting LF, CR and CR + LF as the end of the message */
    } while (!(last == '\n' || (previous == '\r' && last == '\n') || (last == '\r' )));
    if(too_long) {
        telnet.c_flush();
        return "";
    }
    else {
        string message = telnet.stringFlush();
        if(last == '\n') {
            message.pop_back();
            if(previous == '\r')
                message.pop_back();
        } else if(last == '\r') {
            message.pop_back();
        }

        return message;

    }
}
void telnet_serve(std::shared_ptr<TCPConnection> telnet) {
    RemotePlayerCollection rplayers;
    while (true) {
        try {
            Listener listener({telnet->get_socket()});
            bool heard = (listener.listen(TELNET_TIMEOUT) != -1);
            if(heard) {
                string prompt_message = telnet_receive_message(*telnet);
                /* If the message is correct, otherwise just read the next one */
                if (prompt_message != "") {
                    /* empty variables for parsing output */
                    string host;
                    string arguments;
                    int port;
                    string command_name;
                    int player_id;
                    int hour;
                    int minutes; 
                    int time_to_die;

                    /* whether PLAY, PAUSE, QUIT or TITLE */
                    if (rplayers.parse_ppqt(prompt_message, command_name, player_id)) {
                        if (!rplayers.active(player_id)) {
                            telnet->sendMessage("ERROR: wrong id\r\n");
                        }
                        else {
                            if (command_name == "PLAY")
                                telnet->sendMessage(rplayers.get(player_id)->play() + "\r\n");
                            else if (command_name == "PAUSE")
                                telnet->sendMessage(rplayers.get(player_id)->pause() + "\r\n");
                            else if (command_name == "TITLE") {
                                std::shared_ptr<RemotePlayer> rp = rplayers.get(player_id);
                                string message(rp->title() + "\r\n");
                                telnet->sendMessage(message);
                            }
                            else if (command_name == "QUIT") {
                                rplayers.get(player_id)->quit();
                            }
                            else {
                                assert(false);
                            }
                        }
                    }
                    else if (rplayers.parse_at(
                                prompt_message,
                                hour,
                                minutes,
                                time_to_die,
                                host,
                                arguments,
                                port)) {
                        rplayers.insert(host, port, arguments, telnet, hour, minutes, time_to_die);
                    }
                    else if (rplayers.parse_start(prompt_message, host, arguments, port)) {
                        rplayers.insert(host, port, arguments, telnet);
                    }
                    else {
                        telnet->sendMessage("ERROR: wrong command\r\n");
                    }
                }
            }
            rplayers.cleanup();
        } catch (ConnectionError &e) {
            telnet->sendMessage("ERROR: connection: " + string(e.what()) + "\r\n");
            return;
        }
        catch (ServerClosed &e) {
            telnet->mock();
        }
        catch (ParseException &e) {
            telnet->sendMessage("ERROR: parsing error: " + string(e.what()) + "\r\n");
        }
        catch (std::exception &e) {
            telnet->sendMessage(string("ERROR: ") + string(e.what()) + string("\r\n"));
        }
    }
}

}
using namespace radioacek;
int main(int argc, char **argv) {
    uint16_t port;
    switch (argc) {
        case 1:
            port = 0;
            break;
        case 2: {
                    const char *c_port = argv[1];
                    port = parse_port(string(c_port));
                    break;
                }
        default: {
                     cerr << "Wrong arguments, should be ./master port" << endl;
                     exit(1);
                 }
    }
    TCPMediator gate;
    uint16_t new_port = gate.get_port();
    if (port == 0)
        cerr << "Listening on port: " << new_port << endl;
    while (true) {
        std::shared_ptr<TCPConnection> telnet(new TCPConnection(gate));
        thread telnet_server(telnet_serve, telnet);
        telnet_server.detach();
    }
    return 0;
}
