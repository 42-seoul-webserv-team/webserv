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
	    return;
  	}
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
			return ;

		pos = read_str.find('\n');
	}

	if (length < BUFFER_SIZE)
	{
		if (this->mRequest.getStatus() != COMPLETE)
		{
			if (!read_str.empty())
			{
				this->mRequest.set(read_str);
				if (this->mRequest.getStatus() != COMPLETE)
					throw std::runtime_error(""); // connectionException(400);
			}
			else
				throw std::runtime_error(""); // connectionException(400);
		}
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
	std::string type = this->mRequest.getContentType();
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
	std::string type = this->mRequest.getContentType();
	size_t pos = type.find(';');
	if (pos == std::string::npos)
		return ;

	std::string boundary = type.substr(pos + 1);
	pos = boundary.find('=');
	if (pos == std::string::npos)
		return ;
	
	std::string start = boundary.substr(pos + 1);
	std::string end = start + "--";

	boundary.erase(pos);
	if (ft::trim(boundary) != "boundary")
		return ;

	std::string body = this->mRequest.getBody();
	if (body.empty())
		return ;

	std::vector<std::string> line = ft::split(body, "\r\n");
	std::vector<std::string>::iterator it = line.begin();
	while (it != line.end())
	{
		if (*it == end)
			return ;
		
		if (*it != start)
			return ; // connectionException(400);
		it++;

		if (it == line.end())
			return ; // connectionException(400);

		std::vector<std::string> header = ft::split(*it);
		if (header.size() != 4)
			return ; // connectionException(400);

		if (header[0] != "Content-Disposition:"
				|| header[1] != "form-data;")
			return ; // connectionException(400);

		pos = header[2].find('=');
		if (pos == std::string::npos
				|| header[2].substr(0, pos) != "name")
			return ; // connectionException(400);

		pos = header[3].find('=');
		if (pos == std::string::npos
				|| header[3].substr(0, pos) != "filename")
			return ; // connectionException(400);

		this->mUpload.push_back(std::vector<std::string>());
		this->mUpload.back().push_back(header[3].substr(pos + 1));

		it++;
		if (it == line.end())
			return ; // connectionException(400);

		header = ft::split(*it);
		if (header.size() != 2)
			return ; // connectionException(400);

		if (header[0] != "Content-Type:")
			return ;

		this->mUpload.back().push_back(header[1]);

		it++;
		if (it == line.end())
			return ;

		header = ft::split(*it);
		if (!header.empty())
			return ;

		it++;
		std::string content;
		while (it != line.end()
				&& *it != start
				&& *it != end)
		{
			content += *it + "\n";
			it++;
		}
		this->mUpload.back().push_back(content);
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
		for (size_t i = 0; i < this->mUpload.empty(); i++)
		{
			std::cout << "\t\tFile Name: " << this->mUpload[i][0] << std::endl;
			std::cout << "\t\tContent-Type: " << this->mUpload[i][1] << std::endl;
			std::cout << "\t\tContent: " << this->mUpload[i][2] << std::endl;
		}
		std::cout << "\t}" << std::endl;
	}
	this->mRequest.printAll();
	std::cout << std::endl;
}

