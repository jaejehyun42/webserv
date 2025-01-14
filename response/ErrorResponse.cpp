#include <string>
#include <unordered_map>
#include <fstream>
#include "ErrorResponse.hpp"

ErrorResponse::ErrorResponse(){}

ErrorResponse::~ErrorResponse(){}

void    ErrorResponse::setMessage(const std::unordered_map<int, std::string>& data){
    _message = "HTTP/1.1 " + data.at(__statusCode) + " " + data.at(__reasonPhrase) + "\r\n";
    _message += "Connection: closed\r\n";
    if (data.at(__path).empty()){ //error page가 없는경우
        _message += "Content-Type: text/html\r\n";
        std::string tmpMessage;
        tmpMessage += "<!DOCTYPE html>\n"
                "<head>\n"
                    "   <title>" + data.at(__statusCode) + " " + data.at(__reasonPhrase) +"</title>\n"
                "</head>\n"
                "<body>\n"
                    "   <h1>404</h1>\n"
                    "   <p>" + data.at(__statusCode) + " " + data.at(__reasonPhrase) + "</p>\n"
                "</body>\n"
                "</html>";
        std::ostringstream oss;
        oss<<tmpMessage.size();
        _message += "Content-Length: " + oss.str() + "\r\n\r\n";
        _message += tmpMessage;
        return ;
    }
    //error page가 있는경우
    _message += "Content-Type: " + data.at(__contentType) + "\r\n";
    std::ifstream ifs(data.at(__path), std::ios::binary);
    std::ostringstream oss;
    if (!ifs){
        std::cerr<<"Error: error page\n"; //여기서 에러나면?
        return ;
    }
    ifs.seekg(0, std::ios::end);
    std::streampos size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    oss<<size;
    _message += "Content-Length: " + oss.str() + "\r\n\r\n";

    std::string buf(size, '\0');
    if (ifs.read(&buf[0], size)){ //에러나면? 
        ifs.close();
        return ;
    }
    _message += buf;
    ifs.close();
}

const std::string    ErrorResponse::getMessage(){
    return (_message);
}