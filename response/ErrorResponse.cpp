#include <string>
#include <unordered_map>
#include <fstream>
#include "ErrorResponse.hpp"

ErrorResponse::ErrorResponse(){}

ErrorResponse::~ErrorResponse(){}

void    ErrorResponse::setMessage(const std::unordered_map<int, std::string>& data){
    _message = "HTTP/1.1 " + data.at(__statusCode) + " " + data.at(__reasonPhrase) + "\r\n\r\n";
    if (data.at(__path).empty()){
        _message += data.at(__statusCode) + " " + data.at(__reasonPhrase);
    }else{
        std::ifstream ifs(data.at(__path), std::ios::binary);
        if (!ifs)
            std::cerr<<"Error: error page\n";
        ifs.seekg(0, std::ios::end);
        std::streampos size = ifs.tellg();
        ifs.seekg(0, std::ios::beg);
        std::string buf(size, '\0');
        ifs.read(&buf[0], size);
        _message += buf;
        ifs.close();
    }
}

const std::string    ErrorResponse::getMessage(){
    return (_message);
}