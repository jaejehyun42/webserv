#ifndef __ERRORRESPONSE_H__
#define __ERRORRESPONSE_H__

#include <string>
#include "ResponseManager.hpp"

class ErrorResponse{
    public:
        ErrorResponse();
        ~ErrorResponse();
        void            setMessage(const std::unordered_map<int, std::string>& data);
        const std::string    getMessage();
    private:
        std::string          _message;
        // void    _setContentLength();
        void    f(const std::unordered_map<int, std::string>& data);
};
#endif