//
// Created by rm on 28.05.16.
//

#ifndef SIECI2_PLAYER_H
#define SIECI2_PLAYER_H

#include<string>
#include "TCPConnection.h"
#include <boost/regex.hpp>
#include<poll.h>

#include "UDPConnection.h"
#include "Timer.h"
#include <ctime>





class Player {
public:
    struct pollfd poll_array[2]; //first one is master, second radio server

    size_t received_data = 0;
    size_t received_meta = 0;
    size_t meta_size = 0;
    static const int timeout = 5000;
    bool playing;
    bool read_metadata;
    int outdesc;
    TCPConnection radio_connection;
    UDPConnection master_connection;
    unsigned long data_segment_size;
    string stream_title;

    Player(int desc, bool read_metadata = true)
            : radio_connection(), playing(true) {
       outdesc = desc;
       received_data = 0;
       received_meta = 0;
       meta_size = 0;
       this->read_metadata = read_metadata;
       for (int i = 0; i < 2; i++) {
          poll_array[i].fd = -1;
          poll_array[i].events = POLLIN;
          poll_array[i].revents = 0;
       }
    }

    void findMaster(uint16_t listen_port) {
       master_connection.serve(listen_port);

    }

    void connect(string server_name, string path, uint16_t server_port) {

       string init;
       init += "GET " + path + " HTTP/1.0\r\n";
       init += "Host: " + server_name + "\r\n";
       init += "User-Agent: MPlayer 2.0-728-g2c378c7-4build1\r\n";
       init += "Icy-MetaData:" + to_string(read_metadata) + "\r\n";
       init += "\r\n";
       this->radio_connection.connect(server_name.c_str(), to_string(server_port).c_str());
       this->radio_connection.sendMessage(init);
       string answer;
       try {
          size_t received = 0;
          while (received < 4)
             received += radio_connection.receiveMessage(4 - received);
          answer = string(radio_connection.flush(), received);
          string end_seqence("\r\n\r\n");
          while (answer.substr(answer.length() - 4, 4) != end_seqence) {
             radio_connection.receiveMessage(1);
             answer += string(radio_connection.flush(), 1);
          }
       } catch (ServerClosedError &e) {
          std::cerr << "Radio server closed" << std::endl;
          exit(0);
       }
       catch (ConnectionError &e) {
          std::cerr << "Server sent a wrong message, couldn't parse it" << std::endl;
          exit(1);
       }

       // Let's parse this
       using boost::regex;
       using boost::smatch;
       smatch result;

       if (read_metadata) {
          regex server_answer_exp(".*(?:ICY|HTTP/1.0|HTTP/1.1) (\\d{3}).*icy-metaint:(\\d+).*\r\n");
          if (!regex_match(answer, result, server_answer_exp))
             throw (ConnectionError("Server did not send the correct answer"));

          int server_response_code;
          try {
             server_response_code = std::stoi(string(result[1]));
          } catch (exception &e) {
             throw ConnectionError("Something very bad happend while parsing server response");
          }
          if (server_response_code != 200)
             throw (ConnectionError("Wrong response code: should be: 200, was: " + to_string(server_response_code)));
          this->data_segment_size = std::stoul(string(result[2]));
       }
       else {
          regex server_answer_exp(".*(?:ICY|HTTP/1.0|HTTP/1.1) (\\d{3}).*\r\n");
          if (!regex_match(answer, result, server_answer_exp))
             throw (ConnectionError("Server did not send the correct answer"));

          int server_response_code = std::stoi(string(result[1]));
          if (server_response_code != 200)
             throw (ConnectionError("Wrong response code: should be: 200, was: " + to_string(server_response_code)));
       }
    }

    void play() {
       poll_array[0].fd = master_connection.get_socket();
       poll_array[1].fd = radio_connection.get_socket();
       while (true) {
          poll_array[0].revents = 0;
          poll_array[1].revents = 0;

          Timer t((double)timeout/1000.0);
          poll(poll_array, 2, timeout);
          using std::cerr;
          using std::endl;
          if (poll_array[0].revents & POLL_IN) {
             if(t.check()) {
                throw ServerClosedError("Server timeout");
             }
             string command(master_connection.receiveMessage(6));
             if (command == string("PLAY")) {
                playing = true;
             }
             else if (command == string("PAUSE")) {
                playing = false;
             }
             else if (command == string("TITLE")) {
                master_connection.sendMessage(title());
             }
             else if (command == string("QUIT")) {
                exit(0);
             }
             else {
                std::cerr << "Wrong command" << std::endl;
             }

          }
          if (poll_array[1].revents & POLL_IN) {
             play_fragment();
             t.reset();
          }
          if (!(poll_array[0].revents & POLL_IN) && !(poll_array[1].revents & POLL_IN)) {
             throw ServerClosedError("Radio server not responding");
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
                received_data += radio_connection.receiveMessage(data_segment_size - received_data);
                // std::cerr << "received music part: " << received << std::endl;
                if (playing) {
                   size_t len = radio_connection.size();
                   write(outdesc, radio_connection.flush(), len);
                }
                else
                   radio_connection.flush();
                //   std::cerr << "received whole music part: " << std::endl;
             } else {

                if (meta_size == 0) {
                   radio_connection.receiveMessage(1);
                   meta_size = (size_t) (radio_connection.flush()[0] * 16);
                }
                else {
                   received_meta += radio_connection.receiveMessage(size_t(meta_size - received_meta));
                   //  std::cerr << "received meta part: " << received_meta << std::endl;
                   string metadata = radio_connection.stringFlush();
                   //  std::cerr << "Receivde whole metadata, size: " << metadata.size() << std::endl;
                   //   std::cerr << "Received metada: " << metadata << std::endl;
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
             size_t len = radio_connection.size();
             write(outdesc, radio_connection.flush(), len);
          }

    }
};


#endif //SIECI2_PLAYER_H
