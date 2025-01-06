#include "Response.hpp"
#include <iostream>
#include <unistd.h>

Response::Response(int fd, Request& req, ServConf& conf) : _fd(fd), _req(req), _conf(conf) {}

Response::~Response(){}

void    Response::checkRequest(){

}

void    Response::response(){
    StatusLine      statusLine;
    Header          header;
    Body            body;
    string          message;

    message = statusLine.getMessage() + "\r\n" + header.getMessage() + "\r\n" + body.getMessage();

    if (write(_fd, message.c_str(), message.size()) == -1)
        cerr<<"Response write error!";
}