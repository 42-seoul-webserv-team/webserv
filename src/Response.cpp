#include "Response.hpp"

Response::Response(void)
{
	this->mCode = "200";
	this->mStatusMsg = "OK";
	this->mConnectionHeader = false;
}

Response::~Response(void)
{

}

std::string Response::getCode(void)
{
	return this->mCode;
}

std::string Response::getStatusMsg(void)
{
	return this->mStatusMsg;
}

bool Response::getConnectionHeader(void)
{
	return this->mConnectionHeader;
}

std::string Response::getContentType(void)
{
	return this->mContentType;
}

std::string Response::getContentLength(void)
{
	return this->mContentLength;
}

std::string Response::getRedirection(void)
{
	return this->mRedirection;
}

std::string Response::getServerName(void)
{
	return this->mServerName;
}

std::string Response::getBody(void)
{
	return this->mBody;
}

void Response::setCode(unsigned int code)
{
	this->mCode = ft::toString(code, 10);
}

void Response::setStatusMsg(std::string const & msg)
{
	this->mStatusMsg = msg;
}

void Response::setConnectionHeader(void)
{
	this->mConnectionHeader = true;
}

void Response::setContentType(std::string const & type)
{
	this->mContentType = type;
}

void Response::setRedirection(std::string const & path)
{
	this->mRedirection = path;
}

void Response::setServerName(std::string const & server)
{
	this->mServerName = server;
}

void Response::setBody(std::string const & body)
{
	this->mContentLength = ft::toString(body.size(), 10);
	this->mBody = body;
}
