#ifndef __RESPONSEMANAGER_H__
#define __RESPONSEMANAGER_H__

#include "Response.hpp"
#include <string>
#include <unordered_map>

enum eData{
    __statusCode,
    __reasonPhrase,
    __hostName,
    __keepAlive,
    __path,
    __autoindex,
    __body,
    __contentType,
    __contentTypeDefault,
    __requestMethod,
    __requestBody,
    __cgiPass,
    __cgiEnvData // requestCgiPath, requestQuery, requestContentLength, requestContentType
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
        void        _setHeaderData();
        void        _setHost();
        void        _setPath();
        void        _setRequestData();
		void        _setRequestCgiEnv();

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