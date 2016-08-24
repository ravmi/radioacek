#include <iostream>
#include "TCPConnection.h"
#include "Player.h"
#include <fstream>

#include <fcntl.h>

using namespace std;
using std::stoi;
#define PORT_LIMIT 65535
int main(int argc, char **argv) {
   if (argc != 7) {
      cerr << "Wrong number of arguments, should be: ./player host path r-port file m-port md" << endl;
      exit(1);
   }

   const char *c_server_name = argv[1];
   const char *c_path = argv[2];
   const char *c_server_port = argv[3];
   const char *c_file = argv[4];
   const char *c_listen_port = argv[5];
   const char *c_metadata = argv[6];

   string server_name(c_server_name);
   int server_port;
   try {
      server_port = stoi(c_server_port);
   } catch (exception &e) {
      cerr << "Wrong server port" << endl;
      exit(1);
   }
   if (server_port < 0 || server_port > PORT_LIMIT) {
      cerr << "Wrong server port" << endl;
      exit(1);
   }

   int listen_port;
   try {
      listen_port = stoi(c_listen_port);
   } catch (exception &e) {
      cerr << "Wrong listen port" << endl;
      exit(1);
   }
   if (listen_port < 0 || listen_port > PORT_LIMIT) {
      cerr << "Wrong listen port" << endl;
      exit(1);
   }
   string s_metadata(c_metadata);
   bool metadata;
   if (s_metadata == "yes")
      metadata = true;
   else if (s_metadata == "no")
      metadata = false;
   else {
      cerr << "Read metadata? yes/no" << endl;
      exit(1);
   }

   int outdesc = 1;
   if (string(c_file) != string("-")) {
      if ((outdesc = open(c_file, O_RDWR | O_CREAT, 0777)) < 0) {
         cerr << "Couldn't create a file" << endl;
         exit(1);
      }

   }

   string path(c_path);


   Player rad(outdesc);
   try {
      rad.connect(server_name, path, (uint16_t) server_port);
      rad.findMaster((uint16_t) listen_port);
   } catch(ServerClosedError& e) {
      cerr << e.what() << endl;
      exit(0);
   }
   catch (exception &e) {
      cerr << e.what() << ", are you sure arguments are ok?" << endl;
      exit(1);
   }
   try {
      rad.play();
   } catch(ServerClosedError &e) {
      cerr << e.what() << endl;
      exit(0);
   }
   catch (exception &e) {
      cerr << e.what() << " probably server fault" << endl;
      exit(1);
   }
}

