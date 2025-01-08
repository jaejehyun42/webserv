#include "GeneralHeader.hpp"
#include <time.h>
#include <sstream>
#include <iomanip>

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

void  GeneralHeader::_setDate(){ //e.g Date: Tue, 07 Jan 2025 05:29:17 GMT
    std::time_t         now = std::time(NULL);
    struct tm*          gmt = gmtime(&now);
    std::ostringstream  dateStream;
    dateStream << std::put_time(gmt, "%a, %d %b %Y %H:%M:%S GMT");
    std::cout << dateStream.str();
}

string  GeneralHeader::getMessage() const{
    return (_message);
}