#include "ConnectionException.hpp"

ConnectionException::ConnectionException(std::string const & errorMsg, eHTTPStatus const errorCode)
	: std::logic_error(errorMsg)
	, mErrorCode(errorCode)
{}

ConnectionException::~ConnectionException() throw()
{}

eHTTPStatus ConnectionException::getErrorCode(void) const
{
	return this->mErrorCode;
}