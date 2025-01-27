#include <sys/stat.h>
#include <errno.h>
#include <exception>
#include <map>
#include <vector>
#include <fstream>
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
        if (_data.find(__redirectionCode) != _data.end()){
            _makeRedirectionMessage();
            return ;
        }
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
    _checkRequest();
    _setPath();
    _setContentType();
    _setCgiEnv();
}

const std::map<int, std::string>& ResponseManager::_setErrorData(const std::string& errCode, const std::string& reasonPhrase){
    _data[__statusCode] = errCode;
    _data[__reasonPhrase] = reasonPhrase;
    if (_data.find(__connection) == _data.end())
        _checkRequestConnection();

    const std::map<long, string>& errPageMap = _sb.getErrorPage();
	std::map<long, string>::const_iterator it = errPageMap.find(strtol(_data[__statusCode].c_str(), 0, 10));
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

void    ResponseManager::_checkPathIsDir(std::string& path, struct stat& pathStatus, const LocBlock* locationBlock){
    std::string           savedPath = path;
    int                   savedErrno = errno;
    std::vector<string>   indexs;
    
    if (!S_ISDIR(pathStatus.st_mode))
        return ;
    if (!locationBlock)
        throw std::runtime_error("404");
    indexs = locationBlock->getIndex();
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
    if (locationBlock->getAutoindex()){
        _data[__autoindex] = "on";
        _data[__contentType] = "text/html";
        return ;
    }
    throw std::runtime_error("403");
}

void    ResponseManager::_setPath(){
    string      path = _req.getPath();
    struct stat pathStatus;
    std::map<string, LocBlock>::const_iterator it; // it->first : locationIdentifier, it->second : locationBlock
    size_t i = path.find(".py");
//location 블럭 식별
    if (path == "")
        path = "/";
    if (i != std::string::npos && \
        ((path.size() > i + 3 && path[i + 3] == '/') || path.substr(i, path.size()) == ".py")){
        it =  _sb.getPathIter(".py"); //py block
        if (it == _sb.getPath().end())
            throw std::runtime_error("400");
    }
    else{
        it = _sb.getPathIter(path); // others
        if (_data[__requestMethod] == "POST" || _data[__requestMethod] == "DELETE") //post나 delete인데 py블럭이 아니면 400 error
            throw std::runtime_error("400");
    }
    //root 매핑
    if (it == _sb.getPath().end() && _sb.getRoot().size()){//매핑되는 로케이션 블록이 없을 경우. 서버 루트 사용.
        if (_sb.getReturn().first.size()){ //Redirection
            _data[__redirectionCode] = _sb.getReturn().first;
            _data[__redirectionBody] = _sb.getReturn().second;
            throw std::runtime_error(_data[__redirectionCode]);
        }
        if (_sb.getRoot() != "/")
            path.insert(0,_sb.getRoot());
        _data[__path] = path;
        _data[__root] = _sb.getRoot();
    }
    else{
        if (it->second.getReturn().first.size()){ //Redirection
            _data[__redirectionCode] = it->second.getReturn().first;
            _data[__redirectionBody] = it->second.getReturn().second;
            throw std::runtime_error(_data[__redirectionCode]);
        }
        //허용 메서드 체크
        if (_data[__requestMethod] == "GET" && (it->second.getMethod(0) == false))
            throw std::runtime_error("405"); //not allowed
        else if (_data[__requestMethod] == "POST" && (it->second.getMethod(1) == false))
            throw std::runtime_error("405"); //not allowed
        else if (_data[__requestMethod] == "DELETE" && (it->second.getMethod(2) == false))
            throw std::runtime_error("405"); //not allowed
        //root 매핑
        if (it->second.getRoot().empty()){//매핑되는 로케이션블록이 있지만 로케이션블록의 루트가 없는경우. 서버 루트 이용.
            // std::cout<<"1path: "<<path<<"\n";
            if (_sb.getRoot() != "/")
                path.insert(0,_sb.getRoot());
            // std::cout<<"1path: "<<path<<"\n";
            _data[__path] = path;
            _data[__root] = _sb.getRoot();
        }
        else{//매핑되는 로케이션 블록이 있고 루트도 있는 경우. 로케이션 블록의 루트 사용
            // std::cout<<"path: "<<path<<"\n";
            if (it->first == path)
                ;
            else if (it->first == "/" || it->first == ".py")
                path.insert(0,it->second.getRoot());
            else
                path.replace(0, it->first.size(), it->second.getRoot());
            // std::cout<<"path: "<<path<<"\n";
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
        if (it != _sb.getPath().end())
            _checkPathIsDir(path, pathStatus, &it->second);
        else
            _checkPathIsDir(path, pathStatus, NULL);
    }
}

void    ResponseManager::_checkRequestMethod(){
    if (_req.getMethod().size()) //method
        _data[__requestMethod] = _req.getMethod();
}

void    ResponseManager::_checkRequestBody(){
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

    std::map<std::string, std::string> requestHeaderMap = _req.getHeaders();
    if (requestHeaderMap.find("Content-Length") != requestHeaderMap.end())
        _data[__cgiContentLength] += "CONTENT_LENGTH=" + requestHeaderMap["Content-Length"];
    if (requestHeaderMap.find("Content-Type") != requestHeaderMap.end())
        _data[__cgiContentType] += "CONTENT_TYPE=" + requestHeaderMap["Content-Type"];
    if (requestHeaderMap.find("Connection") != requestHeaderMap.end())
        _data[__connection] = requestHeaderMap["Connection"];
}

void    ResponseManager::_checkRequestConnection(){
    std::map<std::string, std::string> requestHeaderMap = _req.getHeaders();
    if (requestHeaderMap.find("Connection") != requestHeaderMap.end())
        _data[__connection] = requestHeaderMap["Connection"];
    else
        _data[__connection] = "closed";
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

void    ResponseManager::_checkRequest(){
    if (_req.getErrorCode().size())
        throw(std::runtime_error(_req.getErrorCode()));
    _checkRequestMethod();
    _checkRequestConnection();
    _checkRequestBody();
}

void    ResponseManager::_makeRedirectionMessage(){
    const int size = 19;
    pair<string, string> table[size];
    table[0] = pair<string, string>("200", "OK");
    table[1] = pair<string, string>("201", "Created");
    table[2] = pair<string, string>("202", "Accepted");
    table[3] = pair<string, string>("203", "Non-Authoritative Information");
    table[4] = pair<string, string>("204", "No Content");
    table[5] = pair<string, string>("205", "Reset Content");
    table[6] = pair<string, string>("206", "Partial Content");
    table[7] = pair<string, string>("207", "Multi-Status");
    table[8] = pair<string, string>("208", "Already Reported");
    table[9] = pair<string, string>("226", "IM Used");
    table[10] = pair<string, string>("300", "Multiple Choices");
    table[11] = pair<string, string>("301", "Moved Permanently");
    table[12] = pair<string, string>("302", "Found");
    table[13] = pair<string, string>("303", "See Other");
    table[14] = pair<string, string>("304", "Not Modified");
    table[15] = pair<string, string>("305", "Use Proxy");
    table[16] = pair<string, string>("306", "Switch Proxy");
    table[17] = pair<string, string>("307", "Temporary Redirect");
    table[18] = pair<string, string>("308", "Permanent Redirect");

    int i = 0;
    for (; i!=size; i++){
        if (_data[__redirectionCode] == table[i].first){
            _data[__reasonPhrase] = table[i].second;
            break;
        }
    }
    if (i == size)
        _data[__reasonPhrase] = "Notknwon Status";
    _message = "HTTP/1.1 " + _data.at(__redirectionCode) + " " + _data.at(__reasonPhrase) + "\r\n";

    if (_data[__redirectionCode][0] == '2' && _data.find(__redirectionBody) != _data.end()){
        _message += "Content-Type: text/html\r\n";
        std::ostringstream oss;
        oss<<_data[__redirectionBody].size();
        _message += "Content-Length: " + oss.str() + "\r\n\r\n";
        _message += _data[__redirectionBody];
    }
    if (_data[__redirectionCode][0] == '3'){
        if (_data.find(__redirectionBody) == _data.end()){
            _data[__contentType] = "text/html";
            ErrorResponse er;
            er.setMessage(_setErrorData("400", "Bad Request"));
            return ;
        }
        _message += "Location: "+ _data[__redirectionBody] + "\r\n\r\n";
    }
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
