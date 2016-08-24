//
// Created by rm on 27.05.16.
//

#ifndef SIECI2_CONNECTIONERROR_H
#define SIECI2_CONNECTIONERROR_H


#include <stddef.h>
#include <string>
#include <exception>
using std::string;
using std::to_string;
using std::exception;
/* This is wrong, server is doing something strange and we should close */
class ConnectionError: public exception{
public:
    string message;
    ConnectionError(string mes):
            message(mes)   {}
    virtual const char* what() const throw()
    {
       return this -> message.c_str();
    }
};


#endif //SIECI2_CONNECTIONERROR_H
