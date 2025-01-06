#include "Header.hpp"

Header::Header(){
    GeneralHeader   generalHeader;
    ResponseHeader  responseHeader;
    EntityHeader    entityHeader;

    _message = generalHeader.getMessage() + responseHeader.getMessage() + entityHeader.getMessage() + "\r\n";
}
// Header(ServConf conf, Request req) : _conf(conf), _req(req), Header(){

// }

Header::~Header(){}

string  Header::getMessage(){
    return (_message);
}