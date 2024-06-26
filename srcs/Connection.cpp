#include "../includes/Connection.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <exception>

eStatus Connection::getReadStatus(void) const
{
	return this->mRequest.getStatus();
}

eMethod Connection::getMethod(void) const
{
	return this->mRequest.getMethod();
}

eProcessType Connection::getProcType(void) const
{
	return this->mProcType;
}

std::string Connection::getContentType(void) const
{
	return this->mRequest.getContentType();
}

std::string Connection::getReqBody(void) const
{
	return this->mRequest.getBody();
}

char * Connection::getAbsolutePath(void) const
{
	return (char *)(this->mAbsolutePath.c_str());
}

void Connection::changeStatus(eStatus const status)
{
	this->mStatus = status;
}

void Connection::fillRequest(void)
{
	std::ifstream file;
	file.open(this->mAbsolutePath.c_str());
	if (!file.is_open())
	{
		throw std::exception(); // 404
	}

	std::string body = "";
	std::string line;
	while (std::getline(file, line))
	{
		body += line;
		body += "\r\n";
	}
	this->mResponse.setBody(body);
	file.close();
}

void Connection::fillRequest(std::vector<std::string> & list)
{
	std::string title = "Index of " + this->mAbsolutePath;
	std::string body = "<!DOCTYPE html>\r\n<html>\r\n<head>\r\n\t<title>" + title + "<title>\r\n<head>";
	body += "<body>\r\n\t<h1>" + title + "<h1>\r\n\t<ul>\r\n";
	for (std::vector<std::string>::iterator it = list.begin(); it != list.end(); it++)
	{
		body += "\t\t<li><a hret=\"" + *it + "\">" + *it + "</a></li>\r\n";
	}
	body += "\t</ul>\r\n</body>\r\n</html>";
	this->mResponse.setBody(body);
}

void Connection::removeFile(void) const
{
	const char * fileName = this->mAbsolutePath.c_str();
	int toCheck = open(fileName, O_RDONLY);
	if (toCheck < 0)
	{
		throw std::exception(); // 404
	}
	close(toCheck);
	if (std::remove(fileName) < 0)
	{
		throw std::exception(); // 500
	}
}