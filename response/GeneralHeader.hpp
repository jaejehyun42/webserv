#ifndef __GENERALHEADER_H_
#define __GENERALHEADER_H_

#include <string>
#include "ResponseManager.hpp"

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
        GeneralHeader(const std::unordered_map<int, std::string>& data);
        ~GeneralHeader();
        string  getMessage() const;
    private:
        const std::unordered_map<int, std::string>& _data;
        string  _message;
        void    _setMessage();
        void    _setConnection();
        void    _setDate();
};

#endif