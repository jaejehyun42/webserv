#include <string>
#include <unordered_map>
#include <fstream>
#include "ErrorResponse.hpp"

ErrorResponse::ErrorResponse(){}

ErrorResponse::~ErrorResponse(){}

void    ErrorResponse::setMessage(const std::unordered_map<int, std::string>& data){
    _message = "HTTP/1.1 " + data.at(__statusCode) + " " + data.at(__reasonPhrase) + "\r\n";
    if (data.find(__path) != data.end()){
        std::ifstream ifs(data.at(__path), std::ios::binary);
        if (!ifs)
            std::cerr<<"Error: error page\n";
        ifs.seekg(0, std::ios::end);
        std::streampos size = ifs.tellg();
        ifs.seekg(0, std::ios::beg);

        char* buf = new char[size];
        ifs.read(buf, size);

        _message += buf;
        delete[] buf;
        ifs.close();
        return ;
    }else{//문구 만들어야함 
        _message = "error";
    }
}

const std::string    ErrorResponse::getMessage(){
    return (_message);
}