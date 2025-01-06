#include "ResponseHeader.hpp"

ResponseHeader::ResponseHeader(const Request& req, const ServConf& conf) : _req(req), _conf(conf) {}

ResponseHeader::~ResponseHeader(){}

string  ResponseHeader::getMessage() const{
    return (_message);
}