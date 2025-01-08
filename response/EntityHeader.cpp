#include "EntityHeader.hpp"

EntityHeader::EntityHeader(const Request& req, const ServConf& conf)
: _req(req), _conf(conf){}

EntityHeader::~EntityHeader(){}

string  EntityHeader::getMessage() const{
    return (_message);
}

void    EntityHeader::_setMessage(){

}

void    EntityHeader::_setContentLength(){ //e.g Content-Length: 615


}

void    EntityHeader::_setContentType(){ //e.g Content-Type: text/html
//기본값은 text/html
}

void    EntityHeader::_setLastModified(){

}