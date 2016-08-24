//
// Created by rm on 27.05.16.
//

#ifndef SIECI2_BUFFEROVERFLOWEXCEPTION_H
#define SIECI2_BUFFEROVERFLOWEXCEPTION_H


#include <stddef.h>
#include <string>
#include <exception>
using std::string;
using std::to_string;
using std::exception;
class BufferOverflowException: public exception{
public:
    string message;
    BufferOverflowException(size_t left, size_t tried_to):
            message(string("Tried to read: ") + to_string(tried_to) + string("free: ") + to_string(left))   {

    }
        virtual const char* what() const throw()
        {
           return this -> message.c_str();
        }
};


#endif //SIECI2_BUFFEROVERFLOWEXCEPTION_H
