#include "Request.hpp"

Request::Request(void)
{
	this->mReadStatus = STARTLINE;
	this->mContentChunk = false;
	this->mContentLength = -1;
	this->mHeaderLength = 0;
}

Request::~Request(void)
{
	mHeader.clear();
}

std::string Request::getMethod(void)
{
	return this->mMethod;
}

std::string Request::getUrl(void)
{
	return this->mUrl;
}

eStatus Request::getStatus(void)
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
		throw std::runtime_error("Start Line Form not valid"); // connectionException(400);

	this->mMethod = words[0];

	if (words[1].size() > URL_LENGTH_MAX)
		throw std::runtime_error("URL too long"); // connectionException(414);

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
			throw std::runtime_error("Version not valid"); // connectionException(400);
		if (words[2].substr(pos + 1) != "1.1")
			throw std::runtime_error("Version not valid"); // connectionException(505);
	}
	else
		throw std::runtime_error("Version not valid"); // connectionException(400);

	this->mReadStatus = HEADER;
}

void Request::setHeader(std::string const & line)
{
	if (this->mHeaderLength > HEADER_LENGTH_MAX)
		throw std::runtime_error(""); // connectionException(400);

	if (line.empty())
	{
		if (this->findHeader("Host").empty())
			throw std::runtime_error(""); // connectionException(400);
		if (this->mMethod == "POST")
		{
			if (!this->findHeader("Content-Length").empty())
			{
				try 
				{
					this->mContentLength = ft::toInt(this->mHeader["Content-Length"], 10);
				} 
				catch (int e) 
				{
					throw std::runtime_error("Content-Length is not number"); // connectionException(400);
				}
			}
			else if (this->findHeader("Transfer-Encoding") == "chunked")
				this->mContentChunk = true;
			else
				throw std::runtime_error("Need Content-Legnth"); // connectionException(411);
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
				throw std::runtime_error("Transfer Chunk Content-length is not number"); // connectionException(400);
			}
			if (this->mContentLength == 0
					&& this->findHeader("Trailer").empty())
				this->mReadStatus = COMPLETE;
		}
		else
		{
			if (this->mContentLength > 0)
			{
				this->mBody += line;
				this->mBody += "\n";
			}
			else
				this->mReadStatus = COMPLETE;
		}
	}
	else
	{
		int length = line.size();
		if (length < this->mContentLength)
		{
			this->mBody += line;
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

std::string Request::getContentType(void)
{
	return this->mContentType;
}

std::string Request::getBody(void)
{
	return this->mBody;
}

void Request::setContentType(std::string const & type)
{
	this->mContentType = type;
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
	}
	std::cout << std::endl;
	std::cout << "\t\tMethod: " << this->mMethod << std::endl;
	std::cout << "\t\tUrl: " << this->mUrl << std::endl;
	std::cout << "\t\tFragment: " << this->mQuery << std::endl;
	std::cout << "\t\tHeader: {" << std::endl;
	for (std::map<std::string, std::string>::iterator it = this->mHeader.begin(); it != this->mHeader.end(); it++)
		std::cout << "\t\t\t" << it->first << ": " << it->second << std::endl;
	std::cout << "\t\t}" << std::endl;
	std::cout << "\t\tBody: " << this->mBody << std::endl;
}
