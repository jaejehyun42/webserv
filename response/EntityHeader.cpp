#include "EntityHeader.hpp"

EntityHeader::EntityHeader(const Request& req, const ServConf& conf) : _req(req), _conf(conf){}

EntityHeader::~EntityHeader(){}

string  EntityHeader::getMessage() const{
    return (_message);
}
