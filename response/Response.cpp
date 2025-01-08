#include "Response.hpp"
#include "StatusLine.hpp"
#include "Header.hpp"
#include "Body.hpp"
#include <iostream>
#include <unistd.h>

Response::Response(const Request& req, const ServConf& conf, const int& servBlockIdx)
: _req(req), _conf(conf), _servBlockIdx(servBlockIdx){

}

Response::~Response(){}

string    Response::getMessage(){
    return (_message);
}

void    Response::_setMessage(){
    int             errCode = 0;
    StatusLine      statusLine(_req, _conf, &errCode, _servBlockIdx); 
    Header          header(_req, _conf, &errCode);
    Body            body(_req, _conf, &errCode);

    string          message = statusLine.getMessage() + header.getMessage() + "\r\n" + body.getMessage();
}