//
// Created by rm on 26.05.16.
//

#ifndef SIECI2_CONNECTION_H
#define SIECI2_CONNECTION_H


class Connection {


public:
    int nsocket;
    Connection() {
       nsocket = -1;
    }

    int get_socket() {
       return nsocket;
    }

};


#endif //SIECI2_CONNECTION_H
