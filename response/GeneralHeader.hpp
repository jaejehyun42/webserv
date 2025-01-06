#ifndef __GENERALHEADER_H_
#define __GENERALHEADER_H_


#include <string>
#include "Header.hpp"
using namespace std;

/* synopsis
general header:
                   Cache-Control            ; Section 14.9
                      | Connection               ; Section 14.10
                      | Date                     ; Section 14.18
                      | Pragma                   ; Section 14.32
                      | Trailer                  ; Section 14.40
                      | Transfer-Encoding        ; Section 14.41
                      | Upgrade                  ; Section 14.42
                      | Via                      ; Section 14.45
                      | Warning                  ; Section 14.46
*/

class GeneralHeader{
    public:
        GeneralHeader();
        ~GeneralHeader();
        string  getMessage();
    private:
        string  _message;
};

#endif