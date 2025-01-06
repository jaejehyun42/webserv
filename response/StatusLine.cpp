#include "StatusLine.hpp"

StatusLine::StatusLine(const Request& req, const ServConf& conf) : _req(req), _conf(conf) {}

StatusLine::~StatusLine(){}

void    StatusLine::setMessage(){
    // check req method
    // check req url
    // check req path
    // check req query
    // check req version
    // check req headers
    // check req body
    _message = _httpVersion + " " + _statusCode + " " + _reasonPhrase + "\r\n";
}

string  StatusLine::getMessage(){
    return (_message);
}

