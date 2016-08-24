//
// Created by rm on 28.05.16.
//

#ifndef SIECI2_SERVERCLOSEDERROR_H
#define SIECI2_SERVERCLOSEDERROR_H
#include <string>
/* This is ok, server timed out or closed the connection */
class ServerClosedError {
public:
    std::string message;
    ServerClosedError(std::string mes):
            message(mes)   {}
    virtual const char* what() const throw()
    {
       return this -> message.c_str();
    }
};


#endif //SIECI2_SERVERCLOSEDERROR_H
