#pragma once
#ifndef __CONNECTION_EXCEPTION_H__
#define __CONNECTINO_EXCEPTION_H__

#include "enum.hpp"

#include <string>
#include <stdexcept>

class ConnectionException : public std::logic_error
{
public:
	ConnectionException(std::string const & errorMsg, eHTTPStatus const errorCode);
	~ConnectionException() throw();
	eHTTPStatus getErrorCode(void) const;
private:
	eHTTPStatus mErrorCode;
};

#endif