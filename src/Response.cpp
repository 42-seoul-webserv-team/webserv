#include "Response.hpp"
#include <iostream>

Response::Response(void)
{
	this->mCode = "200";
	this->mStatusMsg = "OK";
}

Response::~Response(void)
{
	this->mCode.clear();
	this->mStatusMsg.clear();
	this->mContentType.clear();
	this->mContentLength.clear();
	this->mServerName.clear();
	this->mBody.clear();
}

void Response::clear(void)
{
	this->mCode = "200";
	this->mStatusMsg = "OK";
	this->mContentType.clear();
	this->mContentLength.clear();
	this->mServerName.clear();
	this->mBody.clear();
}

std::string Response::getCode(void)
{
	return this->mCode;
}

std::string Response::getStatusMsg(void)
{
	return this->mStatusMsg;
}

std::string Response::getContentType(void)
{
	return this->mContentType;
}

std::string Response::getContentLength(void)
{
	return this->mContentLength;
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

void Response::setContentType(std::string const & type)
{
	this->mContentType = type;
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

void Response::printAll(void)
{
	std::cout << "\t* Print Response!" << std::endl;
	std::cout << "\t\tCode: " << this->mCode << std::endl;
	std::cout << "\t\tStatus Msg: " << this->mStatusMsg << std::endl;
	std::cout << "\t\tServer Name: " << this->mServerName << std::endl;
	std::cout << "\t\tContent-Type: " << this->mContentType << std::endl;
	std::cout << "\t\tContent-Length: " << this->mContentLength << std::endl;
	std::cout << "\t\tBody size: " << this->mBody.size() << std::endl;
	//std::cout << "\t\tBody: " << this->mBody << std::endl;
}
