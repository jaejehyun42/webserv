#ifndef __STATUSLINE_H__
#define __STATUSLINE_H__


#include <string>
using namespace std;

/* synopsis

status line:
    Status-Line = HTTP-Version SP Status-Code SP Reason-Phrase CRLF

*/

class StatusLine{
    public:
        StatusLine();
        ~StatusLine();
        string  getMessage();
    private:
        string  _message;
};
#endif