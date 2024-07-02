#include "Connection.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <sstream>
#include <iostream>
#include <exception>
#include <sys/socket.h>

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

Response Connection::getResponse(void) const
{
	return this->mResponse;
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
	
	while (file.good())
		body.push_back(file.get());

	if (!body.empty())
		body.pop_back();

	this->mResponse.setBody(body);
	file.close();
}

void Connection::fillRequest(std::vector<std::string> & list)
{
	std::string title = "Index of " + this->mAbsolutePath;
	std::string body = "<!DOCTYPE html><html><head><meta charset=\"uft-8\"><title>" + title + "</title></head>";
	body += "<body><h1>" + title + "<h1><ul>";
	for (std::vector<std::string>::iterator it = list.begin(); it != list.end(); it++)
	{
		body += "<li><a href=\"" + *it + "\">" + *it + "</a></li>";
	}
	body += "</ul></body></html>";
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
		this->addEnv(envp);
		char ** CGIenvp = this->convert(envp);
		char * argv[] = {
			const_cast<char *>(this->mCGI.c_str()),
			NULL
		};
		int ret = execve(argv[0], argv, CGIenvp);
		exit(ret);
	}

	this->mStatus = PROC_CGI;
	close(this->mCGIfd[1]);
	kque.addEvent(this->mCGIfd[0], this);
	gettimeofday(&this->mCGIstart, NULL);
}

char ** Connection::convert(std::map<std::string, std::string> env)
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

void Connection::uploadFiles(void)
{
	int success = 0;
	int fail = 0;

	for (size_t j = 0; j < this->mUpload.size(); j++)
	{
		int size = this->mUpload[j].size();
		std::string file_name = "Untitle";
		
		try {
			for (int i = 0; i < size - 1; i++)
			{
				size_t pos = this->mUpload[j][i].find(':');
				if (pos == std::string::npos)
					continue ;
				std::string header = this->mUpload[j][i].substr(0, pos);
				if (header == "Content-Disposition" 
						&& this->mUpload[j][i].find("filename=") != std::string::npos)
				{
					file_name = this->mUpload[j][i].substr(this->mUpload[j][i].find("filename=") + 9);
					if (!file_name.empty() && file_name.front() == '\"')
					{
						file_name.erase(0, 1);
						pos = file_name.find('\"');
						if (pos == std::string::npos)
							throw file_name;
						file_name = file_name.substr(0, pos);
						if (file_name.empty())
							throw file_name;
					}
				}
			}
			std::string path = this->getAbsolutePath();
			if (path.back() == '/')
				path.pop_back();
			
			std::string subject;
			std::string extension = "";
			size_t pos = file_name.rfind('.');
	
			if (pos == std::string::npos || pos == 0)
				subject = file_name;
			else
			{
				subject = file_name.substr(0, pos);
				extension = file_name.substr(pos);
			}
	
			std::string file = path + "/" + subject + extension;
			int file_nbr = 0;
	
			while (access(file.c_str(), F_OK) == 0)
			{
				file = path + "/" + subject + ft::toString(file_nbr, 10) + extension;
				file_nbr++;
			}

			std::ofstream output;
			output.open(file, std::fstream::out);
			if (!output.is_open())
				throw file;
			output << this->mUpload[j].back() << std::endl;
			output.close();
			success++;
		} catch (std::string & e) {
			fail++;
		}
	}

	std::string body;
	body = "Total: " + ft::toString(success + fail, 10) + ", ";
	body += "Success: " + ft::toString(success, 10) + ", ";
	body += "Fail: " + ft::toString(fail, 10);
	this->mResponse.setBody(body);
}

void Connection::isTimeOver(void) const
{
	struct timeval now;
	gettimeofday(&now, NULL);
	if (((now.tv_usec - this->mCGIstart.tv_usec) / 1000000.0) > CGI_OVERTIME)
	{
		kill(this->getCGIproc(), SIGKILL);
		throw ConnectionException("CGI Time out", GATEWAY_TIMEOUT);
	}
}

Connection::Connection(void)
{
	this->mSocket = -1;
	this->mServerPort = -1;
	this->mServer = -1;
	this->mStatus = STARTLINE;
	this->mProcType = NONE;
}

Connection::Connection(int socket, int svr_port)
{
	this->mSocket = socket;
	this->mServerPort = svr_port;
	this->mServer = -1;
	this->mStatus = STARTLINE;
	this->mProcType = NONE;
	this->renewTime();
}

Connection::~Connection(void)
{
	this->mServer = -1;
	this->mAbsolutePath.clear();
	this->mUpload.clear();
	this->mRemainStr.clear();
	this->mCGI.clear();
}

void Connection::setAccept(int socket, int port)
{
	this->mSocket = socket;
	this->mServerPort = port;
	this->renewTime();
}

void Connection::renewTime(void)
{
	gettimeofday(&this->mTime, NULL);
}

int Connection::getSocket(void)
{
	return this->mSocket;
}

int Connection::getServer(void)
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

void Connection::setServer(int  svr)
{
	if (svr == -1)
		return ;
	
	this->mServer = svr;
}

void Connection::readRequest(void)
{
	if (this->mRequest.getStatus() == COMPLETE)
		return ;

	char buffer[BUFFER_SIZE];
	int length = read(this->mSocket, buffer, BUFFER_SIZE);

  	if (length == 0)
		return ;
	else if (length == -1)
		throw ManagerException("Connection closed, Cannot read");

	buffer[length] = '\0';
	this->renewTime();
	std::string read_str = this->mRemainStr + buffer;

	size_t pos = read_str.find('\n');
	while (pos != std::string::npos)
	{
		std::string line = read_str.substr(0, pos);
	
		if (line.back() == '\r')
		{
			line.pop_back();
			read_str.erase(0, pos + 1);
		}
		else
			read_str.erase(0, pos);
		
		this->mRequest.set(line);

		if (this->mRequest.getStatus() == COMPLETE)
		{
			this->mRemainStr.clear();
			return ;
		}

		pos = read_str.find('\n');
	}

	this->mRemainStr = read_str;
}

void Connection::closeSocket(void)
{
	if (this->mSocket != -1)
	{
		close(this->mSocket);
		this->mSocket = -1;
	}
	this->mServerPort = -1;
	this->mServer = -1;
	this->mRequest.clear();
	this->mResponse.clear();
	this->mAbsolutePath.clear();
	this->mUpload.clear();
	this->mRemainStr.clear();
	this->mStatus = STARTLINE;
	this->mProcType = NONE;
	this->mCGI.clear();
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
	gettimeofday(&now, NULL);
	if (now.tv_usec - this->mTime.tv_usec > REQ_OVERTIME)
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
		throw ConnectionException("Upload Body not match format", BAD_REQUEST);
	std::string line = body.substr(0, pos);
	if (line != start)
		throw ConnectionException("Upload Body not match format", BAD_REQUEST);
	
	bool flag = true;	
	while (!body.empty())
	{
		pos = body.find("\r\n");
		if (pos == std::string::npos)
		{
			if (body == end && flag == true)
				break ;
			else
				throw ConnectionException("Upload Body not match format", BAD_REQUEST);
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
			throw ConnectionException("Upload Body not match format", BAD_REQUEST);
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

void Connection::addEnv(std::map<std::string, std::string> & envp)
{
	envp["AUTH_TYPE"] = "";
	envp["QUERY_STRING"] = "";
	envp["REMOTE_ADDR"] = "";
	envp["REMOTE_HOST"] = "";
	envp["REMOTE_IDENT"] = "";
	envp["REMOTE_USER"] = "";
	envp["SCRIPT_NAME"] = "";
	envp["SERVER_NAME"] = ""; // PATH_TRANSLATED랑 동일
	envp["GATEWAY_INTERFACE"] = "CGI/1.1";
	envp["SERVER_PROTOCOL"] = "HTTP/1.1";
	envp["SERVER_SOFTWARE"] = "webserv/1.0";
	envp["PATH_TRANSLATED"] = this->mAbsolutePath;

	std::string length;
	std::stringstream ss;
	eMethod method = this->mRequest.getMethod();
	switch(method)
	{
		case GET :
			envp["REQUEST_METHOD"] = "GET";
			break;
		case POST :
			ss << this->mRequest.getContentLength();
			ss >> length;
			envp["CONTENT_LENGTH"] = length;
			envp["REQUEST_METHOD"] = "POST";
			envp["CONTENT_TYPE"] = this->mRequest.getContentType();
			break;
		case DELETE :
			envp["REQUEST_METHOD"] = "DELETE";
			break;
		default:
			envp["REQEUST_METHOD"] = "UNKNOWN";
			break;
	}

	std::string url = this->mRequest.getUrl();
	std::size_t pos = url.find("//");
	url = url.substr(pos + 2, url.size() - pos);
	pos = url.find("/");
	url = url.substr(pos + 1, url.size() - pos);
	envp["PATH_INFO"] = url;
}
