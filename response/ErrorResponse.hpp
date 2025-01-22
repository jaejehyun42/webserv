#ifndef __ERRORRESPONSE_H__
#define __ERRORRESPONSE_H__

#include <string>
#include "ResponseManager.hpp"

class ErrorResponse{
    public:
        ErrorResponse();
        ~ErrorResponse();
        void            setMessage(const std::map<int, std::string>& data);
        const std::string    getMessage();
    private:
        std::string          _message;
		void    _makeErrorMessage(const std::map<int, std::string>& data);
		void    _makeErrorMessageFromErrorPage(std::ifstream& ifs, const std::map<int, std::string>& data);
		void    _makeErrorMessageBody(std::string& body, const std::string& statusCode, const std::string& reasonPhrase);

		void	_setContentTypeHeader(std::string& header);
        void	_setContentTypeHeader(std::string& header, const std::string& type);
		void    _setConnectionHeader(std::string& header, const std::string& connectionVal);
        void	_setContentLengthHeader(std::string& header, const size_t& size);
        void	_setContentLengthHeader(std::string& header, const std::streampos& size);
};
#endif
