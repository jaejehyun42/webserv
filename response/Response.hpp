#ifndef __RESPONSE_H__
#define __RESPONSE_H__

#include "../request/Request.hpp"
#include "../config/ServConf.hpp"

/* synopsis

Response      = Status-Line                 ;
				*(( general-header          ;
				|   response-header         ;
				|   entity-header ) CRLF)   ;
				CRLF
				[ message-body ] 

*/

class Response{
	public:
		Response(const Request& req, const ServConf& conf, const int& servBlockIdx);
		~Response();
		const string&   getMessage();
	private:
		string    _message;
};

#endif
