#include "GeneralHeader.hpp"

GeneralHeader::GeneralHeader(const Request& req, const ServConf& conf) : _req(req), _conf(conf){
    _setMessage();
}

GeneralHeader::~GeneralHeader(){}

void  GeneralHeader::_setMessage(){
    _setConnection();
    _setDate();
}

void  GeneralHeader::_setConnection(){
}

void  GeneralHeader::_setDate(){
}

string  GeneralHeader::getMessage() const{
    return (_message);
}