#include "Connection.hpp"

Connection::Connection(void)
{
	this->mSocket = -1;
	this->mServerPort = -1;
	this->mServer = nullptr;
	this->mValidStatus = STARTLINE;
	this->mType = NONE;
	this->renewTime();
}

Connection::Connection(int socket, int svr_port)
{
	this->mSocket = socket;
	this->mServerPort = svr_port;
	this->mServer = nullptr;
	this->mValidStatus = STARTLINE;
	this->mType = NONE;
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
			throw std::runtime_error("Request message not enough"); // connectionException(400);
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
			throw std::runtime_error("Request message not enough"); // connectionException(400);
	}
	else
		this->mRemainStr = read_str;
	
}

void Connection::writeResponse(void)
{

}

void Connection::close(void)
{
	if (this->mSocket != -1)
		::close(this->mSocket);
}
/*
void Connection::access(void)
{

}
*/
bool Connection::checkMethod(std::string const & method)
{
	return this->mRequest.getMethod() == method;
}
/*
bool Connection::checkCompelte(void)
{

}
*/
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
	return this->mValidStatus < this->mRequest.getStatus();
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

std::string Connection::getMethod(void)
{
	return this->mRequest.getMethod();
}

std::string Connection::getAbsoultePath(void)
{
	return this->mAbsolutePath;
}

void Connection::setStatus(eStatus status)
{
	this->mValidStatus = status;
}

void Connection::setAbsoultePath(std::string const & root, std::string const & url, std::string const & type)
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
		throw std::runtime_error("Cannot find multipart/form-data");

	std::string boundary = type.substr(pos + 1);
	pos = boundary.find('=');
	if (pos == std::string::npos)
		throw std::runtime_error("Boundary not matched format");
	
	std::string start = "--" + boundary.substr(pos + 1);
	std::string end = start + "--";

	boundary.erase(pos);
	if (ft::trim(boundary) != "boundary")
		throw std::runtime_error("Boundary not match format");

	std::string body = this->mRequest.getBody();
	if (body.empty())
		throw std::runtime_error("Upload Body empty");

		
	pos = body.find("\r\n");
	if (pos == std::string::npos)
		throw std::runtime_error("Upload Body not match format"); // connectionException(400);
	std::string line = body.substr(0, pos);
	if (line != start)
		throw std::runtime_error("Upload Body not match format"); // connectionException(400);
	
	bool flag = true;	
	while (!body.empty())
	{
		pos = body.find("\r\n");
		if (pos == std::string::npos)
		{
			if (body == end && flag == true)
				break ;
			else
				throw std::runtime_error("Upload Body not match format"); // connectionException(400);
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
			throw std::runtime_error("Upload Body not match format"); // connectionException(400);
	}
}

eRunType Connection::getType(void)
{
	return this->mType;
}

void Connection::setType(eRunType type)
{
	this->mType = type;
}

eStatus Connection::getStatus(void)
{
	return this->mValidStatus;
}

# include <iostream>

void Connection::printAll(void)
{
	std::cout << "* Print Connection!" << std::endl;
	std::cout << "\tStatus: ";
	switch (this->mValidStatus) {
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
	std::cout << "\tSocket: " << this->mSocket << std::endl;
	std::cout << "\tConnected Port: " << this->mServerPort << std::endl;
	std::cout << "\tAbsolute Path: " << this->mAbsolutePath << std::endl;
	std::cout << "\tRun Type: ";
	switch (this->mType) {
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
	std::cout << std::endl;
}

