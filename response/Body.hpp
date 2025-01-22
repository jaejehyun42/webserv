#ifndef __BODY_H__
#define __BODY_H__

#include <string>
#include <map>

/* synopsis

message body:

*/

class Body{
    public:
        Body(std::map<int, std::string>& data);
        ~Body();
        std::string  getMessage();
    private:
        std::map<int, std::string>& _data;
        std::string _message;

        void    _setMessage();
        void    _setContentLength();

        void    _makeGetMessage();
        void    _makeAutoindexMessage();
        void	_makeStaticFileMessage();
        void    _makePostMessage();
        void    _makeDeleteMessage();

        void    _makeCgiMessage();
        void	_execCgiProc(int* cgiReadFd, int* cgiWriteFd);
        void    _processCgiMessage(pid_t& cgiProc, int* cgiReadFd, int* cgiWriteFd);
};
#endif
