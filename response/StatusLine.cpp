#include <errno.h>
#include <map>
#include "StatusLine.hpp"
#include "ResponseManager.hpp"

StatusLine::StatusLine(std::map<int, std::string>& data)
: _data(data), _httpVersion("HTTP/1.1"){
    _setMessage();
}

StatusLine::~StatusLine(){

}

std::string  StatusLine::getMessage(){
    return (_message);
}

void    StatusLine::_setMessage(){
    if (_data.find(__statusCode) == _data.end())
        _data[__statusCode] = "200";
    if (_data.find(__reasonPhrase) == _data.end())
        _data[__reasonPhrase] = "OK";
    _message = _httpVersion + " " + _data.at(__statusCode) + " " + _data.at(__reasonPhrase) + "\r\n";
}
