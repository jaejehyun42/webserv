#include "ResponseHeader.hpp"

ResponseHeader::ResponseHeader(const std::unordered_map<int, std::string>& data)
: _data(data){
    (void)_data;
    _setMessage();
}

ResponseHeader::~ResponseHeader(){}

string  ResponseHeader::getMessage() const{
    return (_message);
}

void                ResponseHeader::_setMessage(){
    _setServer();
}

void                ResponseHeader::_setEtag(){

}

void                ResponseHeader::_setServer(){
    std::string     _serverName = "jaejehyun's webserv/1.0";
}

void                ResponseHeader::_setAcceptRanges(){

}