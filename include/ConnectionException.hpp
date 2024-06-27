#pragma once
#ifndef __CONNECTION_EXCEPTION_H__
#define __CONNECTINO_EXCEPTION_H__

#include "enum.hpp"
#include "WebServ.hpp"

#include <string>
#include <stdexcept>

class ConnectionException : public std::logic_error
{
public:
	ConnectionException(std::string const & errorMsg, eHTTPStatus const errorCode);
	~ConnectionException();
	eHTTPStatus getErrorCode(void) const;
private:
	eHTTPStatus mErrorCode;
};

#endif