#include "EntityHeader.hpp"

EntityHeader::EntityHeader(const Request& req, const ServConf& conf) : _req(req), _conf(conf){}

EntityHeader::~EntityHeader(){}

string  EntityHeader::getMessage() const{
    return (_message);
}

void    EntityHeader::_setMessage(){

}

void    EntityHeader::_setContentLength(){

}

void    EntityHeader::_setContentType(){

}

void    EntityHeader::_setLastModified(){

}