#ifndef __RESPONSEHEADER_H__
#define __RESPONSEHEADER_H__

#include <string>
#include "ResponseManager.hpp"

/* synopsis

response header:
                   Accept-Ranges           ; Section 14.5        !x
                       | Age                     ; Section 14.6
                       | ETag                    ; Section 14.19 !x
                       | Location                ; Section 14.30
                       | Proxy-Authenticate      ; Section 14.33
                       | Retry-After             ; Section 14.37
                       | Server                  ; Section 14.38 !
                       | Vary                    ; Section 14.44
                       | WWW-Authenticate        ; Section 14.47

*/

class ResponseHeader{
    public:
        ResponseHeader(const std::map<int, std::string>& data);
        ~ResponseHeader();
        string  getMessage() const;
    private:
        const std::map<int, std::string>& _data;
        string  _message;
        void                _setMessage();
        void                _setEtag();
        void                _setServer();
        void                _setAcceptRanges();
};
#endif