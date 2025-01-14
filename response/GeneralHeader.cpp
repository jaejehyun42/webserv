#include "GeneralHeader.hpp"
#include <time.h>
#include <sstream>
#include <iomanip>

GeneralHeader::GeneralHeader(const std::unordered_map<int, std::string>& data)
: _data(data){
    _setMessage();
}

GeneralHeader::~GeneralHeader(){}

void  GeneralHeader::_setMessage(){
    _setDate();
    _setConnection();
}

void  GeneralHeader::_setConnection(){
    if ((_data.find(__keepAlive) != _data.end()) && \
    (strtol(_data.at(__keepAlive).c_str(), 0, 10) > 0)){
        _message += "Connection: keep-alive\r\n";
    }else {
        _message += "Connection: closed\r\n";
    }
}

void  GeneralHeader::_setDate(){ //e.g Date: Tue, 07 Jan 2025 05:29:17 GMT
    std::time_t now = std::time(NULL);
    struct tm*  gmt = gmtime(&now);
    std::ostringstream  dateStream;
    dateStream << std::put_time(gmt, "%a, %d %b %Y %H:%M:%S GMT");
    _message += dateStream.str() + "\r\n";
}

string  GeneralHeader::getMessage() const{
    return (_message);
}