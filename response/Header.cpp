#include "Header.hpp"

Header::Header(const Request& req, const ServConf& conf) : _req(req), _conf(conf){
    GeneralHeader   generalHeader(_req, _conf);
    ResponseHeader  responseHeader(_req, _conf);
    EntityHeader    entityHeader(_req, _conf);

    _message = generalHeader.getMessage() + responseHeader.getMessage() + entityHeader.getMessage() + "\r\n";
}
// Header(ServConf conf, Request req) : _conf(conf), _req(req), Header(){

// }

Header::~Header(){}

string  Header::getMessage(){
    return (_message);
}