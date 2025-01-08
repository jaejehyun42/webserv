#ifndef __RESPONSEHEADER_H__
#define __RESPONSEHEADER_H__


#include <string>
#include "../request/Request.hpp"
#include "../config/ServConf.hpp"

/* synopsis

response header:
                   Accept-Ranges           ; Section 14.5        !
                       | Age                     ; Section 14.6
                       | ETag                    ; Section 14.19 !
                       | Location                ; Section 14.30
                       | Proxy-Authenticate      ; Section 14.33
                       | Retry-After             ; Section 14.37
                       | Server                  ; Section 14.38 !
                       | Vary                    ; Section 14.44
                       | WWW-Authenticate        ; Section 14.47

*/

class ResponseHeader{
    public:
        ResponseHeader(const Request& req, const ServConf& conf);
        ~ResponseHeader();
        string  getMessage() const;
    private:
        const Request&      _req;
        const ServConf&     _conf;
        string  _message;
        void                _setMessage();
        void                _setEtag();
        void                _setServer();
        void                _setAcceptRanges();
};
#endif