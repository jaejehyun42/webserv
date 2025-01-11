#include <sys/stat.h>
#include <sstream>
#include <time.h>
#include "EntityHeader.hpp"

EntityHeader::EntityHeader(const std::unordered_map<int, std::string>& data)
: _data(data){}

EntityHeader::~EntityHeader(){}

string  EntityHeader::getMessage() const{
    return (_message);
}

void    EntityHeader::_setMessage(){
    struct stat fileStatus;

    if (stat(_data.at(__path).c_str(), &fileStatus) == -1) //응답을 파일에서만 가져온다면. 직접만들지 않고
        throw (std::runtime_error("500")); //internal error
    _setContentType();
    _setContentLength(fileStatus);
    _setLastModified(fileStatus);
}

void    EntityHeader::_setContentLength(struct stat& fileStatus){ //e.g Content-Length: 615
    std::ostringstream oss;
    oss<<fileStatus.st_size;
    _message += "Content-Length: " + oss.str() + "\r\n";
}

void    EntityHeader::_setContentType(){ //e.g Content-Type: text/html
//기본값은 text/html
    _message += "Content-Type: " + _data.at(__contentType) + "\r\n";
}

void    EntityHeader::_setLastModified(struct stat& fileStatus){//Last-Modified: Thu, 01 Jan 2025 12:00:00 GMT
    std::time_t lastModified = fileStatus.st_mtime;
    struct tm*  gmt = gmtime(&lastModified);
    std::ostringstream  dateStream;
    dateStream << std::put_time(gmt, "%a, %d %b %Y %H:%M:%S GMT");
    _message += dateStream.str() + "\r\n";
}