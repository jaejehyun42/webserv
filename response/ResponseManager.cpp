#include <sys/stat.h>
#include <errno.h>
#include <exception>
#include <unordered_map>
#include <vector>
#include "ResponseManager.hpp"
#include "StatusLine.hpp"
#include "Header.hpp"
#include "Body.hpp"
#include "ErrorResponse.hpp"
#include "../request/Request.hpp"

ResponseManager::ResponseManager(const Request& req, const ServConf& conf, const int& servBlockIdx)
:_req(req), _conf(conf), _sb(conf.getServBlock(servBlockIdx)){
    _setMessage();
}

ResponseManager::~ResponseManager(){}

string    ResponseManager::getMessage(){
    // printAllData();
    return (_message);
}

void    ResponseManager::_setMessage(){
    try{
        _setData();
        Body            body(_data);
        StatusLine      statusLine(_data); 
        Header          header(_data);
        _message = statusLine.getMessage() + header.getMessage() + body.getMessage();
    }catch(std::exception& e){
        ErrorResponse er;
        if (std::string(e.what()) == "400"){
            er.setMessage(_setErrorData("400", "Bad Request"));
        }else if (std::string(e.what()) == "403"){
            er.setMessage(_setErrorData("403", "Forbidden"));
        }else if (std::string(e.what()) == "404"){
            er.setMessage(_setErrorData("404", "Not Found"));
        }else if (std::string(e.what()) == "405"){
            er.setMessage(_setErrorData("405", "Method Not Allowed"));
        }else if (std::string(e.what()) == "413"){
            er.setMessage(_setErrorData("413", "Request Entity Too Large"));
        }else if (std::string(e.what()) == "500"){
            er.setMessage(_setErrorData("500", "Internal Server Error"));
        }else if (std::string(e.what()) == "501"){
            er.setMessage(_setErrorData("501", "Not Implemented"));
        }else if (std::string(e.what()) == "505"){
            er.setMessage(_setErrorData("505", "HTTP Version Not Supported"));
        }else{
            er.setMessage(_setErrorData("500", "Internal Server Error"));
        }
        _message = er.getMessage();
    }
}

void    ResponseManager::_setData(){
    _checkRequestError();
    _setMethod();
    _setPath();
    _setBody();
    _setContentType();
    _setConnection();
    _setCgiEnv();
}

const std::unordered_map<int, std::string>& ResponseManager::_setErrorData(const std::string& errCode, const std::string& reasonPhrase){
    _data[__statusCode] = errCode;
    _data[__reasonPhrase] = reasonPhrase;
    _data[__connection] = "closed";

    const std::unordered_map<long, string>& errPageMap = _sb.getErrorPage();
	std::unordered_map<long, string>::const_iterator it = errPageMap.find(strtol(_data[__statusCode].c_str(), 0, 10));
    if (it != errPageMap.end())
        _data[__path] = _sb.getRoot() + "/" + it->second;
    else
        _data[__path] = ""; 

    _setContentType();
    return (_data);
}

void    ResponseManager::_checkPathStatus(const std::string& path, struct stat& pathStatus){
    int savedErrno = errno;

    if (stat(path.c_str(), &pathStatus) == -1){
        if (errno == ENOENT){ //no such file and directory
            errno = savedErrno;
            throw std::runtime_error("404");
        }else if (errno == ENOTDIR){ //not directory
            errno = savedErrno;
            throw std::runtime_error("404");
        }else if (errno == EACCES){ //permission denied          
            errno = savedErrno;
            throw std::runtime_error("403");
        }else{                               
            errno = savedErrno;
            throw std::runtime_error("500");//500 internal server error
        }
    }
}

void    ResponseManager::_checkPathIsDir(std::string& path, struct stat& pathStatus, const LocBlock& locationBlock){
    std::string           savedPath = path;
    int                   savedErrno = errno;
    std::vector<string>   indexs;
    
    if (!S_ISDIR(pathStatus.st_mode))
        return ;
    indexs = locationBlock.getIndex();
    if (indexs.size()){
        for(size_t i=0; i<indexs.size(); i++){
            path = savedPath;
            if (*(path.end() - 1) == '/')
                path += indexs[i];
            else
                path += "/" + indexs[i];
            if (stat(path.c_str(), &pathStatus) == EXIT_SUCCESS){
                _data[__path] = path;
                errno = savedErrno;
                return ;
            }
        }
        errno = savedErrno;
    }
    if (locationBlock.getAutoindex()){
        _data[__autoindex] = "on";
        _data[__contentType] = "text/html";
        return ;
    }
    throw std::runtime_error("404");
}

void    ResponseManager::_setPath(){
    string      path = _req.getPath();
    struct stat pathStatus;
    std::unordered_map<string, LocBlock>::const_iterator it; // it->first : locationIdentifier, it->second : locationBlock
    size_t i = path.find(".py");
//location 블럭 식별
    if (i != std::string::npos && \
        ((path.size() > i + 3 && path[i + 3] == '/') || path.substr(i, path.size()) == ".py")){
        it =  _sb.getPathIter(".py"); //py block
    }
    else{
        it =  _sb.getPathIter(path); // others
        if (_data[__requestMethod] == "POST" || _data[__requestMethod] == "DELETE") //post나 delete인데 py블럭이 아니면 400 error
            throw std::runtime_error("400");
    }

    if (it == _sb.getPath().end() && _sb.getRoot().size()){//매핑되는 로케이션 블록이 없을 경우. 서버 루트 사용.
        if (_sb.getRoot() != "/")
            path.insert(0,_sb.getRoot());
        _data[__path] = path;
        _data[__root] = _sb.getRoot();
    }
    else{
        // if (it->second.getMethod(std::strtol(_data[__requestMethod].c_str(),0,10))) //허용 메서드 체크 
            // throw std::runtime_error("405"); //not allowed
        if (it->second.getRoot().empty()){//매핑되는 로케이션블록이 있지만 로케이션블록의 루트가 없는경우. 서버 루트 이용.
            if (_sb.getRoot() != "/")
                path.insert(0,_sb.getRoot());
            _data[__path] = path;
            _data[__root] = _sb.getRoot();
        }
        else{//매핑되는 로케이션 블록이 있고 루트도 있는 경우. 로케이션 블록의 루트 사용
            if (it->second.getRoot() != "/")
                path.replace(0, it->first.size(), it->second.getRoot());
            _data[__path] = path;
            _data[__root] = it->second.getRoot();
            _data[__locationIdentifier] = it->first;
        }
    }
 //location block이 py블럭인 경우 또는 method가 POST또는 DELETE인 경우
    if (it != _sb.getPath().end() && it->first == ".py"){
        size_t i = path.find(".py");
        // std::cout<<path.substr(0, i + 3);
        _checkPathStatus(path.substr(0, i + 3), pathStatus); //.py까지 경로체크
        if (it->second.getCgipass().size())
            _data[__cgiPass] = it->second.getCgipass();
        else
            throw std::runtime_error("501");
    }
    else{
        _checkPathStatus(path, pathStatus);
        _checkPathIsDir(path, pathStatus, it->second);
    }
}

void    ResponseManager::_setMethod(){
    if (_req.getMethod().size()) //method
        _data[__requestMethod] = _req.getMethod();
}

void    ResponseManager::_setBody(){
    std::string requestBody = _req.getBody(); //body
    if (requestBody.empty())
        return ;
    if (requestBody.size() > (unsigned long)_sb.getMaxSize())
        throw (std::runtime_error("413"));
    _data[__requestBody] = requestBody;
}

void ResponseManager::_setCgiEnv(){
    if (_data.find(__cgiPass) == _data.end())
        return ;

    if (_data.find(__requestMethod) != _data.end())
        _data[__cgiMethod] = "REQUEST_METHOD=" + _data[__requestMethod];
    if (_data.find(__root) != _data.end())
        _data[__cgiRoot] += "DOCUMENT_ROOT=" + _data[__root];

    if (_req.getCgiPath().size())
        _data[__cgiPath] += "PATH_INFO=" + _req.getCgiPath();
    if (_req.getQuery().size())
        _data[__cgiQuery] += "QUERY_STRING=" + _req.getQuery();

    std::unordered_map<std::string, std::string> requestHeaderMap = _req.getHeaders();
    if (requestHeaderMap.find("Content-Length") != requestHeaderMap.end())
        _data[__cgiContentLength] += "CONTENT_LENGTH=" + requestHeaderMap["Content-Length"];
    if (requestHeaderMap.find("Content-Type") != requestHeaderMap.end())
        _data[__cgiContentType] += "CONTENT_TYPE=" + requestHeaderMap["Content-Type"];
}

void    ResponseManager::_setConnection(){
    ostringstream oss;
    if (oss <<_conf.getAliveTime())
        _data[__connection] = oss.str();
}

void    ResponseManager::_setContentType(){
    if (_data.find(__contentType) != _data.end())
        return ;

    size_t i = _data[__path].find_last_of('.');
    if (i == std::string::npos || (i + 1 == _data[__path].size()))
        _data[__contentType] = _conf.getMime("default");
    else
        _data[__contentType] = _conf.getMime(_data[__path].substr(i + 1));
}

void    ResponseManager::_checkRequestError(){
    if (_req.getErrorCode().size())
        throw(std::runtime_error(_req.getErrorCode()));
}

void    ResponseManager::printAllData(){
    if (_data.find(__statusCode)!=_data.end())
        cout<<"StatusCode: "<<_data[__statusCode]<<"\n";
    if (_data.find(__reasonPhrase)!=_data.end())
        cout<<"ReasonPhrase: "<<_data[__reasonPhrase]<<"\n";
    if (_data.find(__hostName)!=_data.end())
        cout<<"HostName: "<<_data[__hostName]<<"\n";
    if (_data.find(__connection)!=_data.end())
        cout<<"Connection: "<<_data[__connection]<<"\n";
    if (_data.find(__path)!=_data.end())
        cout<<"Path: "<<_data[__path]<<"\n";
    if (_data.find(__autoindex)!=_data.end())
        cout<<"AutoIndex: "<<_data[__autoindex]<<"\n";
    if (_data.find(__contentType)!=_data.end())
        cout<<"ContentType: "<<_data[__contentType]<<"\n";
    if (_data.find(__requestMethod)!=_data.end())
        cout<<"RequestMethod: "<<_data[__requestMethod]<<"\n";
    if (_data.find(__requestBody)!=_data.end())
        cout<<"RequestBody: "<<_data[__requestBody]<<"\n";
}