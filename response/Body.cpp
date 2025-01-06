#include "Body.hpp"

Body::Body(const Request& req, const ServConf& conf) : _req(req), _conf(conf){}

Body::~Body(){}

string  Body::getMessage() const{
    return (_message);
}