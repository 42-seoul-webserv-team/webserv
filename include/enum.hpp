#ifndef ENUM_H
# define ENUM_H

enum eStatus
{
	READY, STARTLINE, HEADER, BODY, PROC_CGI, COMPLETE
};

enum eMethod
{
	UNKNOWN, GET, POST, DELETE
};

enum eProcessType
{
	FILE, AUTO_INDEX, CGI
};

enum eRunType
{
	NONE, FILES, AUTOINDEX, CGI, UPLOAD
};

#endif