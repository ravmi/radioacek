//
// Created by rm on 29.05.16.
//

#ifndef SIK_PARSEEXCEPTION_H
#define SIK_PARSEEXCEPTION_H

#include <stddef.h>
#include <string>
#include <exception>


class ParseException : std::exception{

    std::string message;
public:
    ParseException(std::string mes) :
            message(mes) { }

    virtual const char *what() const throw() {
       return this->message.c_str();
    }
};


#endif //SIK_PARSEEXCEPTION_H
