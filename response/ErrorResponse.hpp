#ifndef __ERRORRESPONSE_H__
#define __ERRORRESPONSE_H__

#include <string>
#include "ResponseManager.hpp"

class ErrorResponse{
    public:
        ErrorResponse();
        ~ErrorResponse();
        void            setMessage(const std::unordered_map<int, std::string>& errData);
        const std::string    getMessage();
        std::string          message;
};
#endif