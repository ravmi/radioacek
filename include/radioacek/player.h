#ifndef RADIOACEK_PLAYER_H
#define RADIOACEK_PLAYER_H

#include <boost/regex.hpp>
#include<poll.h>

#include <radioacek/timer.h>

#include <radioacek/tcp_connection.h>
#include <radioacek/udp_connection.h>
#include <radioacek/exceptions.h>
#include <radioacek/listener.h>

#include<signal.h>
#include<string>
#include<iostream>
#include <cerrno>

#include<fstream>

using namespace std;

namespace radioacek {
class Player {
private:
    /* in seconds */
    const double timeout = 5.0;
    unsigned long data_segment_size;
    std::ofstream file_stream;
public:

    size_t received_data = 0;
    size_t received_meta = 0;
    size_t meta_size = 0;
    bool playing = true;
    bool read_metadata = true;
    bool to_file = false;


    radioacek::TCPConnection radio_connection;
    radioacek::UDPConnection controller_connection;
    string stream_title;

    Player(string c_file, bool read_metadata = true) :
        read_metadata(read_metadata) {

            if (string(c_file) != string("-")) {
                to_file = true;
                file_stream.open(c_file);
                if(!file_stream.is_open()) {
                    cerr << "Couldn't create a file" << endl;
                    exit(1);
                }
            }
        }

    void findMaster(uint16_t listen_port) {
        controller_connection.serve(listen_port);
    }

    void output(string message) {
        if(to_file)
            file_stream << message;
        else
            std::cout << message;
    }
    /*
       void hehe(std::string x) {
       output_stream << x;
       }
       */

    void init_response_correct(const string& server_answer) {

        using boost::regex;
        using boost::smatch;
        smatch result;

        int server_response_code;
        if (read_metadata) {
            regex server_answer_regex(
                    ".*(?:ICY|HTTP/1.0|HTTP/1.1) (\\d{3}).*icy-metaint:(\\d+).*\r\n");
            if (!regex_match(server_answer, result, server_answer_regex))
                throw (ConnectionError("Server did not send the correct answer"));

            try {
                server_response_code = std::stoi(string(result[1]));
            } catch (std::exception &e) {
                throw ConnectionError(
                        "Something very bad happend while parsing server response");
            }
            this->data_segment_size = std::stoul(string(result[2]));
        }
        else { /* don't want metadata */
            regex server_answer_regex(".*(?:ICY|HTTP/1.0|HTTP/1.1) (\\d{3}).*\r\n");
            if (!regex_match(server_answer, result, server_answer_regex))
                throw (ConnectionError("Server did not send the correct answer"));

            try {
                server_response_code = std::stoi(string(result[1]));
            } catch (std::exception &e) {
                throw ConnectionError(
                        "Something very bad happend while parsing server response");
            }
        }
        if (server_response_code != 200)
            throw (ConnectionError(
                        "Wrong response code: should be: 200, was: " +
                        to_string(server_response_code)));

    }
    string send_init_message(string server_name, string path, uint16_t server_port) {

        radio_connection.connect(server_name, server_port);
        string init =
            "GET " + path + " HTTP/1.0\r\n"
            "Host: " + server_name + "\r\n"
            "User-Agent: MPlayer 2.0-728-g2c378c7-4build1\r\n"
            "Icy-MetaData:" + to_string(read_metadata) + "\r\n"
            "\r\n";
        radio_connection.sendMessage(init);
        string server_answer;
        try {
            string end_seqence("\r\n\r\n");
            /* Reading any length message that ends with end_sequence */
            radio_connection.receive_message_blocking(4);
            server_answer += radio_connection.stringFlush();
            while (server_answer.substr(
                        server_answer.length() - 4,
                        4) != end_seqence) {
                radio_connection.receiveMessage(1);
                server_answer += radio_connection.stringFlush();
            }
        } catch (ServerClosed &e) {
            std::cerr << "Radio server closed" << std::endl;
            exit(0);
        } catch (std::exception &e) {
            std::cerr << "Server sent a wrong message, couldn't parse it" << std::endl;
            exit(1);
        }
        return server_answer;
    }

    void connect(string server_name, string path, uint16_t server_port) {
        string server_answer = send_init_message(server_name, path, server_port);
        init_response_correct(server_answer);

        // Let's parse this
    }

    void play() {
        Listener listener(
                {
                controller_connection.get_socket(),
                radio_connection.get_socket()
                });

        while (true) {

            Timer t(timeout);
            int active_descriptor_index = listener.listen(timeout);
            if(active_descriptor_index == 0/* controller said something */) {

                if(t.passed()) {
                    throw ServerClosed("Server timeout");
                }
                string command(controller_connection.receiveMessage(6));
                if (command == "PLAY") {
                    playing = true;
                }
                else if (command == "PAUSE") {
                    playing = false;
                }
                else if (command =="TITLE") {
                    controller_connection.sendMessage(title());
                }
                else if (command == "QUIT") {
                    exit(0);
                }
                else {
                    std::cerr << "Wrong command" << std::endl;
                }
            }
            else if(active_descriptor_index == 1/* radio server said something */) {
                play_fragment();
                t.reset();
            }
            else if(active_descriptor_index == -1/* timeout */) {
                throw ServerClosed("Player timeout");

            }

        }
    }

    string title() {
        return this->stream_title;
    }


    void play_fragment() {
        /* if meta_size != 0 we are reading metadata */
        if (read_metadata) {
            if (received_data < data_segment_size) {
                received_data += radio_connection.receiveMessage(
                        data_segment_size - received_data);
                if (playing) {
                    output(radio_connection.stringFlush());
                }
                else 
                    radio_connection.c_flush();
            } else {
                if (meta_size == 0) {
                    radio_connection.receiveMessage(1);
                    meta_size = (size_t) (radio_connection.cFlush()[0] * 16);
                }
                else {
                    received_meta += radio_connection.receiveMessage(
                            size_t(meta_size - received_meta));

                    string metadata = radio_connection.stringFlush();

                    using boost::regex;
                    using boost::smatch;
                    smatch result;
                    regex meta_regex(".*StreamTitle='([^;]*)';.*");
                    if (!regex_match(metadata, result, meta_regex))
                        throw ConnectionError("Wrong metadata");
                    stream_title = result[1];
                    //  std::cerr << "parsed title: " << stream_title << std::endl;
                }

                if (meta_size == received_meta) {
                    received_meta = 0;
                    received_data = 0;
                    meta_size = 0;
                }
            }
        }
        else {
            radio_connection.receiveMessage();
            output(radio_connection.cFlush());
        }
    }
};
}

#endif
