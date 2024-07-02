#ifndef ENUM_H
# define ENUM_H

enum eStatus
{
	STARTLINE, HEADER, BODY, PROC_CGI, COMPLETE
};

enum eMethod
{
	GET, HEAD, POST, DELETE, UNKNOWN
};

enum eProcessType
{
	NONE, FILES, AUTOINDEX, CGI, UPLOAD
};

enum eOpenFail
{
	NOT, ACCESS, ERROR,	BOTH
};

// juhyelee - for Connection Exception
enum eHTTPStatus
{
	OK = 200,
	CREATE = 201,
	PERMANENT_REDIRECT = 308,
	BAD_REQUEST = 400,
	FORBIDDEN = 403,
	NOT_FOUND = 404,
	MATHOD_NOT_ALLOWED = 405,
	REQUEST_TIMEOUT = 408,
	LENGTH_REQUIRED = 411,
	REQUEST_URI_TOO_LONG = 414,
	INTERAL_SERVER_ERROR = 500,
	BAD_GATEWAY = 502,
	GATEWAY_TIMEOUT = 504,
	HTTP_VERSION_NOT_SUP = 505
};

#endif
