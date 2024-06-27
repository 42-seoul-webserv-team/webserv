#pragma once
#ifndef __REDIRECTION_EXCEPTION_H__
#define __REDIRECTION_EXCEPTION_H__

#include <string>
#include <stdexcept>

class RedirectionException : public std::domain_error
{
public :
	RedirectionException(std::string const redirLoc, std::string const serverName);
	~RedirectionException() throw();
	std::string getRedirLoc(void) const;
	std::string getServerName(void) const;
private:
	std::string mRedirectionLocation;
	std::string mServerName;
};

#endif