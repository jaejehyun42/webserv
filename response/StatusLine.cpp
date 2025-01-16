#include <errno.h>
#include <unordered_map>
#include "StatusLine.hpp"
#include "ResponseManager.hpp"

StatusLine::StatusLine(const std::unordered_map<int, std::string>& data)
: _data(data), _httpVersion("HTTP/1.1"), _statusCode(_data.at(__statusCode)), _reasonPhrase(_data.at(__reasonPhrase)){
    _setMessage();
}

StatusLine::~StatusLine(){

}

std::string  StatusLine::getMessage(){
    return (_message);
}

void    StatusLine::_setMessage(){
    _message = _httpVersion + " " + _statusCode + " " + _reasonPhrase + "\r\n";
}
