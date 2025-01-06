#include "GeneralHeader.hpp"

GeneralHeader::GeneralHeader(const Request& req, const ServConf& conf) : _req(req), _conf(conf) {}

GeneralHeader::~GeneralHeader(){}

string  GeneralHeader::getMessage() const{
    return (_message);
}