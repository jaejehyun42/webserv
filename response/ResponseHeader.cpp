#include "ResponseHeader.hpp"

ResponseHeader::ResponseHeader(const std::map<int, std::string>& data)
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
    ;
}

void                ResponseHeader::_setServer(){
    _message += "Server: jaejehyun\r\n";
}
    
void                ResponseHeader::_setAcceptRanges(){
    ;
}
