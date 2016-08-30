#ifndef RADIOACEK_PLAYER_INCLUDE_H
#define RADIOACEK_PLAYER_INCLUDE_H

#include <radioacek/tcp_connection.h>
#include <radioacek/udp_connection.h>

#include <string>
#include <fstream>

namespace radioacek {
class Player {
private:
    /* in seconds */
    const double timeout = 5.0;
    unsigned long data_segment_size;
    std::ofstream file_stream;

    size_t received_data = 0;
    size_t received_meta = 0;
    size_t meta_size = 0;
    bool playing = true;
    bool read_metadata = true;
    bool to_file = false;

    radioacek::TCPConnection radio_connection;
    radioacek::UDPConnection controller_connection;
    std::string stream_title;

    void output(std::string message);
    std::string send_init_message(std::string server_name, std::string path, uint16_t server_port);
    void init_response_correct(const std::string& server_answer);
    void play_fragment();

public:
    Player(std::string c_file, bool read_metadata = true);

    void findMaster(uint16_t listen_port);
    void connect(std::string server_name, std::string path, uint16_t server_port); 
    void play();
    std::string title();
};
}

#endif
