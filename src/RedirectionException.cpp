#include "RedirectionException.hpp"

RedirectionException::RedirectionException(std::string const redirLoc, std::string const serverName)
	: std::domain_error("redirection")
	, mRedirectionLocation(redirLoc)
	, mServerName(serverName)
{}

RedirectionException::~RedirectionException() throw()
{}

std::string RedirectionException::getRedirLoc(void) const
{
	return this->mRedirectionLocation;
}

std::string RedirectionException::getServerName(void) const
{
	return this->mServerName;
}