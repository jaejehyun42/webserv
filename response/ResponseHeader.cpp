#include "ResponseHeader.hpp"

ResponseHeader::ResponseHeader(const Request& req, const ServConf& conf) : _req(req), _conf(conf) {}

ResponseHeader::~ResponseHeader(){}

string  ResponseHeader::getMessage() const{
    return (_message);
}

void                ResponseHeader::_setMessage(){

}

void                ResponseHeader::_setEtag(){

}

void                ResponseHeader::_setServer(){ //e.g. Server: nginx/1.27.3
    std::string     _serverName = "jjh/1.0";
}

void                ResponseHeader::_setAcceptRanges(){

}