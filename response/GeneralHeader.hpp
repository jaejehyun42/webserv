#ifndef __GENERALHEADER_H_
#define __GENERALHEADER_H_


#include <string>
#include "../request/Request.hpp"
#include "../config/ServConf.hpp"

/* synopsis
general header:
                   Cache-Control            ; Section 14.9
                      | Connection               ; Section 14.10 !
                      | Date                     ; Section 14.18 !
                      | Pragma                   ; Section 14.32
                      | Trailer                  ; Section 14.40
                      | Transfer-Encoding        ; Section 14.41
                      | Upgrade                  ; Section 14.42
                      | Via                      ; Section 14.45
                      | Warning                  ; Section 14.46
*/

class GeneralHeader{
    public:
        GeneralHeader(const Request& req, const ServConf& conf);
        ~GeneralHeader();
        string  getMessage() const;
    private:
        const Request&      _req;
        const ServConf&     _conf;
        string  _message;

        void    _setMessage();
        void    _setConnection();
        void    _setDate();
};

#endif