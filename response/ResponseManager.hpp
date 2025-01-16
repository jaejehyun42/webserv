#ifndef __RESPONSEMANAGER_H__
#define __RESPONSEMANAGER_H__

#include "Response.hpp"
#include <string>
#include <unordered_map>

enum eData{
    __statusCode,
    __reasonPhrase,
    __hostName,
    __connection,
    __path,
    __root,
    __locationIdentifier,
    __autoindex,
    __contentLength,
    __contentType,
    __requestMethod,
    __requestBody,
    __cgiPass,
    __cgiMethod,
    __cgiPath,
    __cgiQuery,
    __cgiContentLength,
    __cgiContentType,
    __cgiRoot
};

class ResponseManager{
    public:
        ResponseManager(const Request& req, const ServConf& conf, const int& servBlockIdx);
        ~ResponseManager();
        std::string     getMessage();
        void            printAllData();
    private:
        void        _setData();
        const std::unordered_map<int, std::string>& _setErrorData(const std::string& errCode, const std::string& reasonPhrase);
        void        _setMessage();
        void        _setPath();
		void        _setCgiEnv();
        void        _setBody();
        void        _setMethod();
        void        _setConnection();
        void        _setContentType();

        void    _checkPathIsDir(std::string& path, struct stat& pathStatus, const LocBlock* locationBlock);
        void    _checkPathStatus(const std::string& path, struct stat& pathStatus);
        void    _checkRequestError();


        const Request&      _req;
        const ServConf&     _conf;
        const ServBlock&    _sb;
        std::unordered_map<int,string>  _data;
        std::string                     _message;
};

#endif