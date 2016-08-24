//
// Created by rm on 30.05.16.
//

#ifndef SIK_REMOTEPLAYEREXCEPTION_H
#define SIK_REMOTEPLAYEREXCEPTION_H

#include <stddef.h>
#include <string>
#include <exception>

class RemotePlayerException: public std::exception{

public:
    std::string message;
    int id;
    RemotePlayerException(const int id, std::string mes):
            message(mes), id(id)  {}
    virtual const char* what() const throw()
    {
       return this -> message.c_str();
    }
};


#endif //SIK_REMOTEPLAYEREXCEPTION_H
