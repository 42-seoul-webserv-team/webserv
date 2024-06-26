#include "../includes/Connection.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <exception>
#include <sys/socket.h>

static char ** convert(std::map<std::string, std::string> const env);

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

void Connection::processCGI(Kqueue & kque, std::map<std::string, std::string> envp[])
{
	int pair[2];
	if (socketpair(AF_UNIX, SOCK_STREAM, 0, pair) < 0)
	{
		throw std::exception(); // 500
	}
	fcntl(pair[0], O_NONBLOCK);
	fcntl(pair[1], O_NONBLOCK);

	int child = fork();
	if (child < 0)
	{
		throw std::exception(); // 500
	}
	else if (child == 0)
	{
		dup2(pair[1], STDOUT_FILENO);
		close(pair[0]);
		close(pair[1])
		char ** CGIenvp = convert(envp);
	}

	this->mStatus = PROC_CGI;
	close(pair[1]);
	kque.addEvent(pair[0], this); // 1초 마다 이벤트가 발생했는지 확인해야함
}

static char ** convert(std::map<std::string, std::string> const env)
{
	char ** ret = new char * [env.size() + 1];
	size_t index = 0;
	
	std::map<std::string, std::string>::const_iterator it;
	for (it = env.begin(); it != env.end(); it++)
	{
		std::string str = it->first + "=" + it->second;
		std::size_t size = str.size();
		ret[index] = new char [size + 1];
		for (std::size_t str_index = 0; str_index < size; str_index++)
		{
			ret[index][str_index] = str[str_index];
		}
		ret[index][size] = '\0';
		index++:
	}
	ret[index] = NULL;
	return ret;
}