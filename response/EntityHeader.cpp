#include <sys/stat.h>
#include <sstream>
#include <time.h>
#include "EntityHeader.hpp"

EntityHeader::EntityHeader(const std::map<int, std::string>& data)
: _data(data){
    _setMessage();
}

EntityHeader::~EntityHeader(){}

string  EntityHeader::getMessage() const{
    return (_message);
}

void    EntityHeader::_setMessage(){
    if (_data.find(__contentLength) != _data.end()){ //메세지를 만드는 경우
        _setContentType();
        _setContentLength();
        return ;
    }
    struct stat fileStatus; //파일을 읽은 경우
    if (stat(_data.at(__path).c_str(), &fileStatus) == -1)
        throw (std::runtime_error("500")); //internal error
    _setContentType();
    _setContentLength(fileStatus);
    _setLastModified(fileStatus);
}

void    EntityHeader::_setContentLength(){
    if (_data.find(__contentLength) != _data.end()){
        _message += "Content-Length: " + _data.at(__contentLength) + "\r\n";
        return ;
    }
}
void    EntityHeader::_setContentLength(struct stat& fileStatus){
    std::ostringstream oss;
    oss<<fileStatus.st_size;
    _message += "Content-Length: " + oss.str() + "\r\n";
}

void    EntityHeader::_setContentType(){ //e.g Content-Type: text/html
    if (_data.find(__contentType) != _data.end()){
        _message += "Content-Type: " + _data.at(__contentType) + "\r\n";
    }
}

void    EntityHeader::_setLastModified(struct stat& fileStatus){//e.g. Last-Modified: Thu, 01 Jan 2025 12:00:00 GMT
    std::time_t lastModified = fileStatus.st_mtime;
    struct tm*  gmt = gmtime(&lastModified);
    std::ostringstream  dateStream;
    dateStream << std::put_time(gmt, "%a, %d %b %Y %H:%M:%S GMT");
    _message += "Last-Modified: " + dateStream.str() + "\r\n";
}
