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

std::string Request::getQuery(void)
{
	return this->mQuery;
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
	if (this->mReadStatus == COMPLETE)
		return ;

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

int Request::getBodySize(void)
{
	return this->mBody.size();
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

	if (line == "\r\n")
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
			{
				throw ConnectionException("POST required entity header", MATHOD_NOT_ALLOWED);
			}
			this->mReadStatus = CHECK_TYPE;
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
		if (key == "Host" && !this->mHeader[key].empty())
			throw ConnectionException("Bad Request", BAD_REQUEST);
		this->mHeader[key] = value;
	}

	this->mHeaderLength += line.size();
}

void Request::setStatus(eStatus status)
{
	this->mReadStatus = status;
}

void Request::setBody(std::string const & line)
{
	std::string trim_line = line;
	ft::trim(trim_line);
	if (this->mContentChunk)
	{
		if (trim_line.empty())
			return ;

		if (this->mContentLength <= 0)
		{
			try
			{
				this->mContentLength = ft::toInt(line, 16);
			}
			catch (int e)
			{
				throw ConnectionException("Transfer Chunk Content-length is not number", BAD_REQUEST);
			}
			if (this->mContentLength == 0)
				this->mReadStatus = COMPLETE;
		}
		else
		{
			this->mBody += trim_line;
			this->mContentLength -= static_cast<int>(trim_line.size());
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

std::string Request::getBody(void) const
{
	return this->mBody;
}

void Request::setContentLength(int length)
{
	this->mContentLength = length;
}

bool Request::isChunk(void)
{
	return this->mContentChunk;
}

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
		case CHECK_TYPE:
			std::cout << "CHECK TYPE";
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
		case HEAD:
			std::cout << "HEAD";
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
	std::cout << "\t\tBody Size: " << this->mBody.size() << std::endl;
//	std::cout << "\t\tBody: " << this->mBody << std::endl;
}

int Request::getContentLength(void) const
{
	return this->mContentLength;
}

std::string Request::getContentType(void) const
{
	return this->mContentType;
}
