#include <unordered_map>
#include <exception>
#include <sys/stat.h>
#include <errno.h>
#include <vector>
#include "ResponseManager.hpp"
#include "StatusLine.hpp"
#include "Header.hpp"
#include "Body.hpp"

ResponseManager::ResponseManager(const Request& req, const ServConf& conf, const int& servBlockIdx)
:_req(req), _conf(conf), _sb(conf.getServBlock(servBlockIdx)){
    _setMessage();
}

ResponseManager::~ResponseManager(){}

string    ResponseManager::response(){
    return (_message);
}

void    ResponseManager::_setMessage(){

    try{
        StatusLine      statusLine(_getStatusLineDate()); 
        Header          header(_getHeaderDate());
        Body            body(_getBodyDate());
    }catch(std::exception& e){
        if (e.what() == "400"){

        }else if (e.what() == "403"){

        }else if (e.what() == "404"){

        }else if (e.what() == "413"){

        }else if (e.what() == "500"){

        }
    }
    // _message = statusLine.getMessage() + header.getMessage() + "\r\n" + body.getMessage();
}
enum statusLineData{
    StatusCode,
    ReasonPhrase,
    Path,
    Body
};
std::unordered_map<int, std::string>& ResponseManager::_getStatusLineDate(){
    _checkRequest();
    _checkHost();
    _checkPath();
    _checkBody();
}

std::unordered_map<int, std::string>& ResponseManager::_getHeaderDate(){
}
std::unordered_map<int, std::string>& ResponseManager::_getBodyDate(){
}

void    ResponseManager::_checkRequest(){
    if (_req.getErrorCode().size())
        throw(std::runtime_error(_req.getErrorCode()));
}
void    ResponseManager::_checkHost(){
    unordered_map<string,string> headers = _req.getHeaders();

    if (headers.find("Host") == headers.end())
        throw std::runtime_error("400");
    std::vector<string>::const_iterator it = _sb.getName().begin();
    for (it ; it!=_sb.getName().end() ; it++){
        if (headers["Host"] == *it)
            return ;
    }
    throw std::runtime_error("400");
}
void    ResponseManager::_checkBody(){
    if (_req.getBody().size())
        return ;
    if (_sb.getMaxSize())
        throw (std::runtime_error("413"));
    _statusLineData[Body] = _req.getBody();
}
void    ResponseManager::_checkPath(){
    struct stat urlStatusBuf;
    int saved_errno = errno;
    string path = _req.getPath();
    if (path.empty())
        throw std::runtime_error("400");
/*
    1.서버블록의 location의 root
    2.
*/
    const LocBlock lb =  _sb.getLocation();
    path.erase(path.begin());
    sb.getRoot();

    if (path)
    if (stat(path.c_str(), &urlStatusBuf) == -1){
        if (errno == ENOENT) //no such file and directory
            throw std::runtime_error("404");
        else if (errno == ENOTDIR) //not directory
            throw std::runtime_error("404");
        else if (errno == EACCES) //permission denied
            throw std::runtime_error("403");
        else //500 internal server error
            throw std::runtime_error("500");
        errno = saved_errno;
    }
    _statusLineData[Path] = 0;
}