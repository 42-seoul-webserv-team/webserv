#include "Connection.hpp"
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

std::string Connection::getContentType(void)
{
	return this->mRequest.findHeader("Content-Type");
}

std::string Connection::getReqBody(void) const
{
	return this->mRequest.getBody();
}

char * Connection::getAbsolutePath(void) const
{
	return (char *)this->mAbsolutePath.c_str();
}

int Connection::getCGIproc(void) const
{
	return this->mCGIproc;
}

void Connection::fillRequest(void)
{
	std::ifstream file;
	file.open(this->mAbsolutePath.c_str());
	if (!file.is_open())
	{
		throw ConnectionException("Not exist file", NOT_FOUND);
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
		throw ConnectionException("Not exist file", NOT_FOUND);
	}
	close(toCheck);
	if (std::remove(fileName) < 0)
	{
		throw ConnectionException("Fail to remove file", INTERAL_SERVER_ERROR);
	}
}

void Connection::processCGI(Kqueue & kque, std::map<std::string, std::string> envp)
{
	if (socketpair(AF_UNIX, SOCK_STREAM, 0, this->mCGIfd) < 0)
	{
		throw ConnectionException("Fail to create socket pair", INTERAL_SERVER_ERROR);
	}
	fcntl(this->mCGIfd[0], O_NONBLOCK);
	fcntl(this->mCGIfd[1], O_NONBLOCK);

	this->mCGIproc = fork();
	if (this->mCGIproc < 0)
	{
		throw ConnectionException("Fail to create CGI process", INTERAL_SERVER_ERROR);
	}
	else if (this->mCGIproc == 0)
	{
		dup2(this->mCGIfd[1], STDOUT_FILENO);
		close(this->mCGIfd[0]);
		close(this->mCGIfd[1]);
		char ** CGIenvp = convert(envp);
		char * argv[] = {
			const_cast<char *>(this->mCGI.c_str()),
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

void Connection::isTimeOver(void) const
{
	clock_t currTime = clock();
	double runtime = static_cast<double>(currTime - this->mCGIstart) / CLOCKS_PER_SEC;
	if (runtime >= 1)
	{
		kill(this->getCGIproc(), SIGKILL);
		throw ConnectionException("CGI Time out", GATEWAY_TIMEOUT); // connectionException(504);
	}
}

// 1.0 merge
// #include "Connection.hpp"

Connection::Connection(void)
{
	this->mSocket = -1;
	this->mServerPort = -1;
	this->mServer = nullptr;
	this->mStatus = STARTLINE;
	this->mProcType = NONE;
	this->renewTime();
}

Connection::Connection(int socket, int svr_port)
{
	this->mSocket = socket;
	this->mServerPort = svr_port;
	this->mServer = nullptr;
	this->mStatus = STARTLINE;
	this->mProcType = NONE;
	this->renewTime();
}

Connection::~Connection(void)
{
	this->mUpload.clear();
}

void Connection::renewTime(void)
{
	gettimeofday(&this->mTime, nullptr);
}

int Connection::getSocket(void)
{
	return this->mSocket;
}

Server *Connection::getServer(void)
{
	return this->mServer;
}

int Connection::getPort(void)
{
	return this->mServerPort;
}

std::string Connection::getHost(void)
{
	return this->mRequest.findHeader("Host");
}

void Connection::setServer(Server *svr)
{
	if (svr == nullptr)
		return ;
	
	this->mResponse.setServerName(svr->getServerName());
	this->mServer = svr;
}

void Connection::readRequest(void)
{
	if (this->mRequest.getStatus() == COMPLETE)
		return ;

	char buffer[BUFFER_SIZE];
	int length = read(this->mSocket, buffer, BUFFER_SIZE);

  	if (length <= 0)
	{
    	this->mRequest.set(this->mRemainStr);
		this->mRemainStr.clear();
		if (!this->mRequest.checkBodyComplete())
			throw ConnectionException("Request message not enough", BAD_REQUEST); // connectionException(400);
		return ;
  	}

	std::string read_str = this->mRemainStr + buffer;

	size_t pos = read_str.find('\n');
	while (pos != std::string::npos)
	{
		std::string line = read_str.substr(0, pos);
		
		line.pop_back();
		read_str.erase(0, pos + 1);
		
		this->mRequest.set(line);

		if (this->mRequest.getStatus() == COMPLETE)
			return ;

		pos = read_str.find('\n');
	}

	if (length < BUFFER_SIZE)
	{
		if (!read_str.empty())
			this->mRequest.set(read_str);

		if (!this->mRequest.checkBodyComplete())
			throw ConnectionException("Request message not enough", BAD_REQUEST); // connectionException(400);
	}
	else
		this->mRemainStr = read_str;
	
}

void Connection::writeResponse(void)
{

}

void Connection::closeSocket(void)
{
	if (this->mSocket != -1)
		::close(this->mSocket);
}

bool Connection::checkMethod(eMethod method)
{
	return this->mRequest.getMethod() == method;
}

bool Connection::checkComplete(void)
{
	return this->mStatus == COMPLETE && this->mRequest.getStatus() == COMPLETE;
}

bool Connection::checkOvertime(void)
{
	struct timeval now;
	gettimeofday(&now, nullptr);
	if (now.tv_sec - this->mTime.tv_sec > OVERTIME)
		return true;
	return false;
}

bool Connection::checkStatus(void)
{
	return this->mStatus < this->mRequest.getStatus();
}

bool Connection::checkUpload(void)
{
	std::string type = this->mRequest.findHeader("Content-Type");
	if (type.empty())
		return false;

	size_t pos = type.find(';');
	if (pos == std::string::npos)
		return false;

	std::string multipart = type.substr(0, pos);
	if (ft::trim(multipart) != "multipart/form-data")
		return false;
	return true;
}

std::string Connection::getUrl(void)
{
	return this->mRequest.getUrl();
}

eMethod Connection::getMethod(void)
{
	return this->mRequest.getMethod();
}

void Connection::setStatus(eStatus status)
{
	this->mStatus = status;
}

void Connection::setAbsolutePath(std::string const & root, std::string const & url, std::string const & type)
{
	std::string path = root;

	if (path.back() == '/')
		path.pop_back();

	if (url.front() != '/')
		path += "/" + url;
	else
		path += url;

	this->mAbsolutePath = path;
	this->mResponse.setContentType(type);
}

void Connection::setUpload(void)
{
	std::string type = this->mRequest.findHeader("Content-Type");
	size_t pos = type.find(';');
	if (pos == std::string::npos)
		throw ConnectionException("Cannot find multipart/form-data", BAD_REQUEST);

	std::string boundary = type.substr(pos + 1);
	pos = boundary.find('=');
	if (pos == std::string::npos)
		throw ConnectionException("Boundary not matched format", BAD_REQUEST);
	
	std::string start = "--" + boundary.substr(pos + 1);
	std::string end = start + "--";

	boundary.erase(pos);
	if (ft::trim(boundary) != "boundary")
		throw ConnectionException("Boundary not match format", BAD_REQUEST);

	std::string body = this->mRequest.getBody();
	if (body.empty())
		throw ConnectionException("Upload Body empty", BAD_REQUEST);

		
	pos = body.find("\r\n");
	if (pos == std::string::npos)
		throw ConnectionException("Upload Body not match format", BAD_REQUEST); // connectionException(400);
	std::string line = body.substr(0, pos);
	if (line != start)
		throw ConnectionException("Upload Body not match format", BAD_REQUEST); // connectionException(400);
	
	bool flag = true;	
	while (!body.empty())
	{
		pos = body.find("\r\n");
		if (pos == std::string::npos)
		{
			if (body == end && flag == true)
				break ;
			else
				throw ConnectionException("Upload Body not match format", BAD_REQUEST); // connectionException(400);
		}
		else
		{
			line = body.substr(0, pos);
			body.erase(0, pos + 2);
		}

		if (line == start && flag == true)
		{
			flag = false;
			this->mUpload.push_back(std::vector<std::string>());
		}
		else if (line == end && flag == true)
			break ;
		else if (line.empty() && flag == false)
		{
			flag =  true;
			this->mUpload.back().push_back("");
		}
		else if (flag == false)
			this->mUpload.back().push_back(line);
		else if (flag == true)
			this->mUpload.back().back() += line + "\r\n";
		else
			throw ConnectionException("Upload Body not match format", BAD_REQUEST); // connectionException(400);
	}
}

void Connection::setContentType(std::string const & type)
{
	this->mResponse.setContentType(type);
}

eProcessType Connection::getType(void)
{
	return this->mProcType;
}

void Connection::setType(eProcessType type)
{
	this->mProcType = type;
}

eStatus Connection::getStatus(void)
{
	return this->mStatus;
}

void Connection::setCGI(std::string const & cgi)
{
	this->mCGI = cgi;
}

# include <iostream>

void Connection::printAll(void)
{
	std::cout << "* Print Connection!" << std::endl;
	std::cout << "\tStatus: ";
	switch (this->mStatus) {
		case STARTLINE:
			std::cout << "STARTLINE";
			break ;
		case HEADER:
			std::cout << "HEADER";
			break ;
		case BODY:
			std::cout << "BODY";
			break ;
		case PROC_CGI:
			std::cout << "PROC_CGI";
			break ;
		case COMPLETE:
			std::cout << "COMPLETE";
			break ;
	}
	std::cout << std::endl;
	std::cout << "\tSocket: " << this->mSocket << std::endl;
	std::cout << "\tConnected Port: " << this->mServerPort << std::endl;
	std::cout << "\tAbsolute Path: " << this->mAbsolutePath << std::endl;
	if (this->mProcType == CGI)
		std::cout << "\tCGI: " << this->mCGI << std::endl;
	std::cout << "\tRun Type: ";
	switch (this->mProcType) {
		case NONE:
			std::cout << "NONE" << std::endl;
			break ;
		case FILES:
			std::cout << "FILES" << std::endl;
			break ;
		case AUTOINDEX:
			std::cout << "AUTOINDEX" << std::endl;
			break ;
		case CGI:
			std::cout << "CGI" << std::endl;
			break ;
		case UPLOAD:
			std::cout << "UPLOAD" << std::endl;
			break ;
	}
	if (!this->mUpload.empty())
	{
		std::cout << "\tUpload files: { " << std::endl;
		for (size_t i = 0; i < this->mUpload.size(); i++)
		{
			std::cout << "\t\tUpload Header: {" << std::endl;
			for (size_t j = 0; j < this->mUpload[i].size() - 1; j++)
				std::cout << "\t\t\t" << this->mUpload[i][j] << std::endl;
			std::cout << "\t\t}" << std::endl;
			std::cout << "\t\tUpload Content: " << this->mUpload[i].back() << std::endl;;
		}
		std::cout << "\t}" << std::endl;
	}
	this->mRequest.printAll();
	this->mResponse.printAll();
	std::cout << std::endl;
}

