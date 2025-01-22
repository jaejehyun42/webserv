#include "GeneralHeader.hpp"
#include <time.h>
#include <sstream>
#include <iomanip>

GeneralHeader::GeneralHeader(const std::map<int, std::string>& data)
: _data(data){
    _setMessage();
}

GeneralHeader::~GeneralHeader(){}

void  GeneralHeader::_setMessage(){
    _setDate();
    _setConnection();
}

void  GeneralHeader::_setConnection(){
    if (_data.find(__connection) != _data.end())
        _message += "Connection: " + _data.at(__connection) + "\r\n";
    else
        _message += "Connection: closed\r\n";
}

void  GeneralHeader::_setDate(){ //e.g Date: Tue, 07 Jan 2025 05:29:17 GMT
    std::time_t now = std::time(NULL);
    struct tm*  gmt = gmtime(&now);
    std::ostringstream  dateStream;
    dateStream << std::put_time(gmt, "%a, %d %b %Y %H:%M:%S GMT");
    _message += "Date: " + dateStream.str() + "\r\n";
}

string  GeneralHeader::getMessage() const{
    return (_message);
}
