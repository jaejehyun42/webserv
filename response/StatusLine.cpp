#include "StatusLine.hpp"

StatusLine::StatusLine(const Request& req, const ServConf& conf) : _req(req), _conf(conf) {}

StatusLine::~StatusLine(){}

void    StatusLine::setMessage(){
    Request& r = const_cast<Request&>(_req); //const가 _req 메서드에 없어서 r객체를 임시로 사용합니다.

    // check req method
    string method = r.getMethod();
    if (method != "GET" && method != "POST" && method != "DELETE"){
        // 501: not implemented
    }

    // check req url
    // check req path
    // check req query
    if (r.getQuery().size()){
        // 501: not implemented
    }
    // check req version
    string version = r.getVersion();
    if (version.size() && version != "1.1"){
        // 505: HTTP version Not Supported
    }
    // check req headers
    // check req body
    _message = _httpVersion + " " + _statusCode + " " + _reasonPhrase + "\r\n";
}

string  StatusLine::getMessage(){
    return (_message);
}

