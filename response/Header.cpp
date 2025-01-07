#include "Header.hpp"
#include "GeneralHeader.hpp"
#include "ResponseHeader.hpp"
#include "EntityHeader.hpp"

Header::Header(const Request& req, const ServConf& conf) : _req(req), _conf(conf){
    _setHeader();
}

Header::~Header(){}

string  Header::getMessage(){
    return (_message);
}

void    Header::_setHeader(){
    GeneralHeader   generalHeader(_req, _conf);
    ResponseHeader  responseHeader(_req, _conf);
    EntityHeader    entityHeader(_req, _conf);

    _message = generalHeader.getMessage() + responseHeader.getMessage() + entityHeader.getMessage() + "\r\n";
}