#include "../includes/Connection.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <exception>
#include <sys/socket.h>

static char ** convert(std::map<std::string, std::string> env);

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

void Connection::fillRequestCGI(void)
{
	char buffer[1024];
	std::string ret = "";
	while (read(this->mCGIfd[0], buffer, 1023) > 0)
	{
		buffer[1023] = '\0';
		std::string str = buffer;
		ret += str;
	}
	this->mResponse.setBody(ret);
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

void Connection::processCGI(Kqueue & kque, std::map<std::string, std::string> envp)
{
	if (socketpair(AF_UNIX, SOCK_STREAM, 0, this->mCGIfd) < 0)
	{
		throw std::exception(); // 500
	}
	fcntl(this->mCGIfd[0], O_NONBLOCK);
	fcntl(this->mCGIfd[1], O_NONBLOCK);

	this->mCGIproc = fork();
	if (this->mCGIproc < 0)
	{
		throw std::exception(); // 500
	}
	else if (this->mCGIproc == 0)
	{
		dup2(this->mCGIfd[1], STDOUT_FILENO);
		close(this->mCGIfd[0]);
		close(this->mCGIfd[1]);
		char ** CGIenvp = convert(envp);
		char *argv[] = {
			"/User/juhyelee/cgi_tester",
			NULL
		};
		execve(argv[0], argv, CGIenvp);
	}

	this->mStatus = PROC_CGI;
	close(this->mCGIfd[1]);
	kque.addEvent(this->mCGIfd[0], this); // 1초 마다 이벤트가 발생했는지 확인해야함
	this->mCGIstart = clock();
}

static char ** convert(std::map<std::string, std::string> env)
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
		index++;
	}
	ret[index] = NULL;
	return ret;
}

bool Connection::isTimeOver(void) const
{
	clock_t currTime = clock();
	double runtime = static_cast<double>(currTime - this->mCGIstart) / CLOCKS_PER_SEC;
	return (runtime >= 1);
}