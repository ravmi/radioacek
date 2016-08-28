#ifndef RADIOACEK_EXCEPTIONS_H
#define RADIOACEK_EXCEPTIONS_H


#include <string>
#include <exception>

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

/* This is wrong, server is doing something strange and we should close */
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


class ParseException : std::exception{

    std::string message;
public:
    ParseException(std::string mes) :
            message(mes) { }

    virtual const char *what() const throw() {
       return this->message.c_str();
    }
};


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


#endif
