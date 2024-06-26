#ifndef ENUM_H
# define ENUM_H

enum eStatus
{
	READY, STARTLINE, HEADER, BODY, PROC_CGI, COMPLETE
};

enum eMethod
{
	GET, POST, DELETE
};

enum eProcessType
{
	FILE, AUTO_INDEX, CGI
};

#endif