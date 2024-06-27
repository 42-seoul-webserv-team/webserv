#ifndef ENUM_H
# define ENUM_H

enum eStatus
{
	STARTLINE, HEADER, BODY, PROC_CGI, COMPLETE
};

enum eMethod
{
	UNKNOWN, GET, POST, DELETE
};

enum eProcessType
{
	NONE, FILES, AUTOINDEX, CGI, UPLOAD
};

#endif
