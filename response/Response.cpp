#include "Response.hpp"
#include "StatusLine.hpp"
#include "Header.hpp"
#include "Body.hpp"
#include <iostream>
#include <unistd.h>

Response::Response(Request& req, ServConf& conf) : _req(req), _conf(conf) {}

Response::~Response(){}

void    Response::checkRequest(){

}

void    Response::response(int fd){
    StatusLine      statusLine(_req, _conf);
    Header          header(_req, _conf);
    Body            body(_req, _conf);
    string          message;

    message = statusLine.getMessage() + "\r\n" + header.getMessage() + "\r\n" + body.getMessage();

    if (write(fd, message.c_str(), message.size()) == -1)
        cerr<<"Response write error!";
}