#include <string>
#include <unordered_map>
#include <fstream>
#include "ErrorResponse.hpp"

ErrorResponse::ErrorResponse(){}

ErrorResponse::~ErrorResponse(){}


const std::string    ErrorResponse::getMessage(){
    return (_message);
}

void    ErrorResponse::setMessage(const std::unordered_map<int, std::string>& data){
    _message = "HTTP/1.1 " + data.at(__statusCode) + " " + data.at(__reasonPhrase) + "\r\n";

    if (data.find(__path) != data.end() && data.at(__path).empty()){ //error page가 없는경우
        _makeErrorMessage(data);
        return ;
    }
    std::ifstream ifs(data.at(__path), std::ios::binary);
    if (!ifs) //error page field는 있는데 실제로 존재하지 않는 경우
        _makeErrorMessage(data);
    else //error page가 실제로 존재하는 경우
        _makeErrorMessageFromErrorPage(ifs, data);
}

void    ErrorResponse::_makeErrorMessage(const std::unordered_map<int, std::string>& data){
    std::string header;
    std::string body;

    _setConnectionHeader(header, data.at(__connection));
    _setContentTypeHeader(header);
    _makeErrorMessageBody(body, data.at(__statusCode), data.at(__reasonPhrase));
    _setContentLengthHeader(header, body.size());
    _message += header + "\r\n" + body;
}

void    ErrorResponse::_makeErrorMessageFromErrorPage(std::ifstream& ifs, const std::unordered_map<int, std::string>& data){
    ifs.seekg(0, std::ios::end);
    std::streampos size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    std::string header;
    _setConnectionHeader(header, data.at(__connection));
    _setContentTypeHeader(header, data.at(__contentType));
    _setContentLengthHeader(header, size);

    std::string body(size, '\0');
    ifs.read(&body[0], size);
    if (!ifs){
        ifs.close();
        _makeErrorMessage(data);
        return ;
    }
    ifs.close();
    _message += header + "\r\n" + body;
}

void    ErrorResponse::_makeErrorMessageBody(std::string& body, const std::string& statusCode, const std::string& reasonPhrase){
    body += "<!DOCTYPE html>\n"
            "<head>\n"
                "   <title>" + statusCode + " " + reasonPhrase +"</title>\n"
            "</head>\n"
            "<body>\n"
                "   <h1>" + statusCode + "</h1>\n"
                "   <p>" + statusCode + " " + reasonPhrase + "</p>\n"
            "</body>\n"
            "</html>";
}

void    ErrorResponse::_setContentTypeHeader(std::string& header){
    header += "Content-Type: text/html\r\n";
}

void	ErrorResponse::_setContentTypeHeader(std::string& header, const std::string& type){
    header += "Content-Type: " + type + "\r\n";
}

void    ErrorResponse::_setContentLengthHeader(std::string& header, const size_t& size){
    std::ostringstream oss;
    oss<<size;
    header += "Content-Length: " + oss.str() + "\r\n";
}

void	ErrorResponse::_setContentLengthHeader(std::string& header, const std::streampos& size){
    std::ostringstream oss;
    oss<<size;
    header += "Content-Length: " + oss.str() + "\r\n";
}

void    ErrorResponse::_setConnectionHeader(std::string& header, const std::string& connectionVal){
    header += "Connection: " + connectionVal + "\r\n";
}