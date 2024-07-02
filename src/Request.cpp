#include "Request.hpp"

Request::Request(void)
{
	this->mReadStatus = STARTLINE;
	this->mMethod = UNKNOWN;
	this->mContentChunk = false;
	this->mContentLength = -1;
	this->mHeaderLength = 0;
}

Request::~Request(void)
{
	this->mUrl.clear();
	this->mQuery.clear();
	this->mBody.clear();
	this->mContentType.clear();
	this->mHeader.clear();
}

void Request::clear(void)
{
	this->mReadStatus = STARTLINE;
	this->mMethod = UNKNOWN;
	this->mContentChunk = false;
	this->mContentLength = -1;
	this->mContentType.clear();
	this->mHeaderLength = 0;
	this->mUrl.clear();
	this->mQuery.clear();
	this->mBody.clear();
	this->mHeader.clear();
}

eMethod Request::getMethod(void) const
{
	return this->mMethod;
}

std::string Request::getUrl(void) const
{
	return this->mUrl;
}

eStatus Request::getStatus(void) const
{
	return this->mReadStatus;
}

std::string Request::findHeader(std::string const & key)
{
	return this->mHeader[key];
}
void Request::set(std::string const & line)
{
	if (this->mReadStatus == STARTLINE)
	{
		this->setStartLine(line);
	}
	else if (this->mReadStatus == HEADER)
	{
		this->setHeader(line);
	}
	else if (this->mReadStatus == BODY)
	{
		this->setBody(line);
	}
}

void Request::setStartLine(std::string const & line)
{
	std::vector<std::string> words = ft::split(line);

	if (words.empty() || words.size() != 3)
		throw ConnectionException("Start Line Form not valid", BAD_REQUEST);

	if (words[0] == "GET")
		this->mMethod = GET;
	else if (words[0] == "POST")
		this->mMethod = POST;
	else if (words[0] == "DELETE")
		this->mMethod = DELETE;
	else if (words[0] == "HEAD")
		this->mMethod = HEAD;
	else
		throw ConnectionException("Unkwon Method", BAD_REQUEST);

	if (words[1].size() > URL_LENGTH_MAX)
		throw ConnectionException("URL too Long", REQUEST_URI_TOO_LONG);

	size_t pos = words[1].find('?');
	if (pos != std::string::npos)
	{
		this->mUrl = words[1].substr(0, pos);
		this->mQuery = words[1].substr(pos + 1);
	}
	else
		this->mUrl = words[1];

	pos = words[2].find('/');
	if (pos != std::string::npos)
	{
		if (words[2].substr(0, pos) != "HTTP")
			throw ConnectionException("Version not found", BAD_REQUEST);
		if (words[2].substr(pos + 1) != "1.1")
			throw ConnectionException("Version not valid", HTTP_VERSION_NOT_SUP);
	}
	else
		throw ConnectionException("Version not found", BAD_REQUEST);

	this->mReadStatus = HEADER;
}

void Request::setHeader(std::string const & line)
{
	if (this->mHeaderLength > HEADER_LENGTH_MAX)
		throw ConnectionException("Header Lenghth too long", BAD_REQUEST);

	if (line.empty())
	{
		if (this->findHeader("Host").empty())
			throw ConnectionException("Need Host Header", BAD_REQUEST);
		if (this->mMethod == POST)
		{
			if (!this->findHeader("Content-Length").empty())
			{
				try 
				{
					this->mContentLength = ft::toInt(this->mHeader["Content-Length"], 10);
				} 
				catch (int e) 
				{
					throw ConnectionException("Content-Length is not number", BAD_REQUEST);
				}
			}
			else if (this->findHeader("Transfer-Encoding") == "chunked")
				this->mContentChunk = true;
			else
				throw ConnectionException("Method not allowed", MATHOD_NOT_ALLOWED);
			this->mReadStatus = BODY;
		}
		else
			this->mReadStatus = COMPLETE;
		return ;
	}

	size_t pos = line.find(':');
	if (pos != std::string::npos)
	{	
		std::string key = line.substr(0, pos);
		std::string value = line.substr(pos + 1);
		ft::trim(key);
		ft::trim(value);
		this->mHeader[key] = value;
	}

	this->mHeaderLength += line.size();
}

void Request::setBody(std::string const & line)
{
	if (this->mContentChunk)
	{
		if (this->mContentLength == -1)
		{
			try
			{
				this->mContentLength = ft::toInt(line, 16);
			}
			catch (int e)
			{
				throw ConnectionException("Transfer Chunk Content-length is not number", BAD_REQUEST);
			}
			if (this->mContentLength == 0
					&& this->findHeader("Trailer").empty())
				this->mReadStatus = COMPLETE;
		}
		else
		{
			if (this->mContentLength > 0)
				this->mBody += line + "\r\n";
			else
				this->mReadStatus = COMPLETE;
		}
	}
	else
	{
		int length = line.size();
		if (length < this->mContentLength)
		{
			this->mBody += line + "\r\n";
			this->mContentLength -= length;
		}
		else
		{
			this->mBody += line.substr(0, this->mContentLength);
			this->mContentLength = 0;
			this->mReadStatus = COMPLETE;
		}
	}
}

std::string Request::getBody(void) const
{
	return this->mBody;
}

bool Request::checkBodyComplete(void)
{
	if (this->mReadStatus == COMPLETE)
		return true;
	else if (this->mReadStatus < BODY)
		return false;

	if (this->mHeader["Content-Length"].empty())
		return false;

	this->mReadStatus = COMPLETE;
	return true;
}

# include <iostream>

void Request::printAll(void)
{
	std::cout << "\t* Print Request!" << std::endl;
	std::cout << "\t\tStatus: ";
	switch (this->mReadStatus) {
		case STARTLINE:
			std::cout << "STARTLINE";
			break ;
		case HEADER:
			std::cout << "HEADER";
			break ;
		case BODY:
			std::cout << "BODY";
			break ;
		case COMPLETE:
			std::cout << "COMPLETE";
			break ;
		default:
			break ;
	}
	std::cout << std::endl;
	std::cout << "\t\tMethod: ";
	switch (this->mMethod) {
		case GET:
			std::cout << "GET";
			break ;
		case POST:
			std::cout << "POST";
			break ;
		case DELETE:
			std::cout << "DELETE";
			break ;
		default:
			break ;
	}
	std::cout << std::endl;
	std::cout << "\t\tUrl: " << this->mUrl << std::endl;
	std::cout << "\t\tFragment: " << this->mQuery << std::endl;
	std::cout << "\t\tHeader: {" << std::endl;
	for (std::map<std::string, std::string>::iterator it = this->mHeader.begin(); it != this->mHeader.end(); it++)
		std::cout << "\t\t\t" << it->first << ": " << it->second << std::endl;
	std::cout << "\t\t}" << std::endl;
	std::cout << "\t\tBody: " << this->mBody << std::endl;
}
