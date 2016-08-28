/**
 *   Errors used in the project, some of them are serious and
 *   lead to termination of the program, some should should be catched
 *   and dealt with
 *   @TODO divide then into these groups
 */

#ifndef RADIOACEK_EXCEPTIONS_H
#define RADIOACEK_EXCEPTIONS_H


#include <string>
#include <exception>

/* Serious error */
class BufferOverflowException: public std::exception{
public:
    std::string message;
    BufferOverflowException(size_t left, size_t tried_to):
            message(std::string("Tried to read: ") + std::to_string(tried_to) + std::string("free: ") + std::to_string(left))   {

    }
        virtual const char* what() const throw()
        {
           return this -> message.c_str();
        }
};

/* Serious error */
class ConnectionError: public std::exception{
public:
    std::string message;
    ConnectionError(std::string mes):
            message(mes)   {}
    virtual const char* what() const throw()
    {
       return this -> message.c_str();
    }
};


/* Serious error */
class ParseException : public std::exception{

    std::string message;
public:
    ParseException(std::string mes) :
            message(mes) { }

    virtual const char *what() const throw() {
       return this->message.c_str();
    }
};


/* Serious error */
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


/* Not an actual error, program may continue */
class ServerClosed : public std::exception{
public:
    std::string message;
    ServerClosed(std::string mes):
            message(mes)   {}
    virtual const char* what() const throw()
    {
       return this -> message.c_str();
    }
};


#endif
