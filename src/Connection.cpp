#include "Connection.hpp"

eStatus Connection::getReadStatus(void) const
{
	return this->mRequest.getStatus();
}

int	Connection::getBodySize(void)
{
	return this->mRequest.getBodySize();
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

int Connection::getCGISocket(void)
{
	return this->mCGIfd;
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
		body.erase(body.size() - 1);

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
		std::string el = this->mAbsolutePath + "/" + *it;
		std::string uri = this->mRequest.getUrl();
		if (uri[uri.size() - 1] == '/')
		{
			uri += *it;
		}
		else
		{
			uri = uri + "/" + *it;
		}
		DIR * dir = opendir(el.c_str());
		if (dir == NULL)
		{
			body += "<li><a href=\"" + uri + "\">" + *it + "</a></li>";
		}
		else
		{
			closedir(dir);
			body += "<li><a href=\"" + uri + "/\">" + *it + "/</a><l/li>";
		}
	}
	body += "</ul></body></html>";
	this->mResponse.setBody(body);
}

void Connection::fillRequestCGI(void)
{
	close(this->mCGIfd);
	this->mCGIfd = -1;

	char buffer[BUFFER_SIZE];
	std::string ret;
	int fd = open(this->mCGIfile.c_str(), O_RDONLY);
	if (fd < 0)
		throw ConnectionException("Not found CGI output file", BAD_GATEWAY);

	while (true)
	{
		int length = read(fd, buffer, BUFFER_SIZE - 1);
		if (length <= 0)
			break ;
		buffer[length] = '\0';
		ret += buffer;
	}
	close(fd);
	this->mResponse.setBody(ret);
	this->mCGIproc = -1;
	std::remove(this->mCGIfile.c_str());
	this->mStatus = CGI_COMPLETE;
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
	bool isPost = this->mRequest.getMethod();

	this->mCGIfile = "ouput" + ft::toString(this->mSocket, 10);
	this->mCGIfd = open(this->mCGIfile.c_str(), O_RDWR | O_CREAT, 0644);
	if (this->mCGIfd < 0)
		throw ConnectionException("Fail to create CGI file", INTERAL_SERVER_ERROR);
	fcntl(this->mCGIfd, F_SETFL, O_NONBLOCK);

	int inputFile = -1;
	std::string inputFileName = "";
	if (isPost)
	{
		inputFileName = "input" + ft::toString(this->mSocket, 10);
		inputFile = open(inputFileName.c_str(), O_RDWR | O_CREAT, 0644);
		if (inputFile < 0)
			throw ConnectionException("Fail to open request file", BAD_GATEWAY);
		std::string body = this->mRequest.getBody();
		if (write(inputFile, body.c_str(), body.size()) < 0)
			throw ConnectionException("Fail to send body to CGI", BAD_GATEWAY);
		close(inputFile);
	}

	if (!isPost)
		inputFile = open(this->mAbsolutePath.c_str(), O_RDONLY);
	else
		inputFile = open(inputFileName.c_str(), O_RDONLY);
	if (inputFile < 0)
		throw ConnectionException("Fail to open request file", BAD_GATEWAY);
	this->mCGIproc = fork();
	if (this->mCGIproc < 0)
	{
		throw ConnectionException("Fail to create CGI process", INTERAL_SERVER_ERROR);
	}
	else if (this->mCGIproc == 0)
	{
		dup2(inputFile, STDIN_FILENO);
		dup2(this->mCGIfd, STDOUT_FILENO);
		close(inputFile);
		close(this->mCGIfd);
		this->addEnv(envp);
		char ** CGIenvp = this->convert(envp);
		char * argv[] = {
			const_cast<char *>(this->mCGI.c_str()),
			NULL
		};
		int ret = execve(argv[0], argv, CGIenvp);
		exit(ret);
	}
	close(inputFile);
	if (isPost)
		std::remove(inputFileName.c_str());
	kque.addCGI(this->mCGIfd, this);
	this->mStatus = PROC_CGI;
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
		int size = this->mUploadInfo[j].size();
		std::string file_name = "Untitle";
		
		try
		{
			std::string path = this->getAbsolutePath();
			if (*path.rbegin() == '/')
				path.erase(path.size() - 1);

			if (access(path.c_str(), F_OK) != 0)
				throw "fail";
			DIR *dir = opendir(path.c_str());
			if (dir == NULL)
				throw "fail";
			closedir(dir);
			if (access(path.c_str(), W_OK) != 0)
				throw "fail";

			for (int i = 0; i < size; i++)
			{
				size_t pos = this->mUploadInfo[j][i].find(':');
				if (pos == std::string::npos)
					continue ;
				std::string header = this->mUploadInfo[j][i].substr(0, pos);
				if (header == "Content-Disposition")
				{
					pos = this->mUploadInfo[j][i].find("filename=");
					if (pos == std::string::npos)
						continue ;
					file_name = this->mUploadInfo[j][i].substr(pos + 9);
					if (!file_name.empty() && *file_name.begin() == '\"')
					{
						file_name.erase(0, 1);
						pos = file_name.find('\"');
						if (pos == std::string::npos)
							throw "fail";
						file_name = file_name.substr(0, pos);
						if (file_name.empty())
							file_name = "Untitle" ;
					}
				}
			}
			
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
			output.open(file.c_str(), std::fstream::out);
			if (!output.is_open())
				throw "fail";
			for (size_t i = 0; i < this->mUpload[j].size(); i++)
			{
				output << this->mUpload[j][i];
			}
			output.close();
			success++;
		} catch (char const* str) {
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
	if ((now.tv_sec - this->mCGIstart.tv_sec ) + (( now.tv_usec - this->mCGIstart.tv_usec ) / 1000000) > CGI_OVERTIME)
	{
		throw ConnectionException("CGI Time out", GATEWAY_TIMEOUT);
	}
}

Connection::Connection(void)
{
	this->mSocket = -1;
	this->mServerPort = -1;
	this->mServer = -1;
	this->mStatus = STARTLINE;
	this->mUploadStatus = STARTLINE;
	this->mProcType = NONE;
	this->mCGIfd = -1;
	this->mCGIproc = -1;
}

Connection::Connection(int socket, int svr_port)
{
	this->mSocket = socket;
	this->mServerPort = svr_port;
	this->mServer = -1;
	this->mStatus = STARTLINE;
	this->mUploadStatus = STARTLINE;
	this->mProcType = NONE;
	this->mCGIfd = -1;
	this->mCGIproc = -1;
	this->renewTime();
}

Connection::~Connection(void)
{
	this->mAbsolutePath.clear();
	this->mUpload.clear();
	this->mRemain.clear();
	this->mCGI.clear();
}

void Connection::setMessage(std::string const & message)
{
	this->mResponse.setBody(message);
}

std::string Connection::getMessage(void)
{
	return this->mResponse.getBody();
}

void Connection::setServerName(std::string const & svr)
{
	this->mResponse.setServerName(svr);
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

bool Connection::checkUploadSize(int size)
{
	int body = 0;
	for (size_t file = 0; file < this->mUpload.size(); file++)
	{
		body += mUploadStart.size();
		if (body > size)
			return true;
		for (size_t info = 0; info < this->mUploadInfo[file].size(); info++)
		{
			body += this->mUploadInfo[file][info].size();
			if (body > size)
				return true;
		}
		body += this->mUpload[file].size();
		if (body > size)
			return true;
	}
	body += mUploadEnd.size();
	if (body > size)
		return true;
	return false;
}

void Connection::readRequest(void)
{
	if (this->mRequest.getStatus() == COMPLETE)
		return ;

	char buffer[BUFFER_SIZE];
	int length = read(this->mSocket, buffer, BUFFER_SIZE - 1);

	if (length == -1)
		throw ManagerException("Connection closed, Cannot read");
	else if (this->mRemain.empty() && length == 0)
		return ;
	else if (length != 0)
	{
		for (int i = 0; i < length; i++)
			this->mRemain.push_back(buffer[i]);
		this->renewTime();
	}

	if (this->mProcType != UPLOAD)
		this->setBody();
	if (this->mRequest.getStatus() == CHECK_TYPE)
	{
		if (this->checkUpload())
			this->mProcType = UPLOAD;
		this->mRequest.setStatus(BODY);
	}
	if (this->mProcType == UPLOAD)
		this->setUpload();
	else
		this->setBody();

}

void Connection::setBody(void)
{
	while (!this->mRemain.empty()
			&& this->mRemain.end() != find(this->mRemain.begin(), this->mRemain.end(), '\n'))
	{
		std::string line;
		while (this->mRemain.front() != '\n')
		{
			line.push_back(this->mRemain.front());
			this->mRemain.pop_front();
		}
		line.push_back(this->mRemain.front());
		this->mRemain.pop_front();	

		this->mRequest.set(line);

		switch (this->mRequest.getStatus())
		{
			case CHECK_TYPE:
				return ;
			case COMPLETE:
				this->mRemain.clear();
				return ;
			default:
				break;
		}
	}

	if (!this->mRemain.empty()
			&& this->mRequest.getStatus() == BODY
			&& !this->mRequest.isChunk())
	{
		std::string line;
		while (!this->mRemain.empty())
		{
			line.push_back(this->mRemain.front());
			this->mRemain.pop_front();
		}
		this->mRequest.set(line);
	}
}

void Connection::closeSocket(void)
{
	if (this->mSocket != -1)
	{
		close(this->mSocket);
		this->mSocket = -1;
	}
	if (this->mCGIproc != -1)
	{
		kill(this->mCGIproc, SIGKILL);
		this->mCGIproc = -1;
	}
	if (this->mCGIfd != -1)
	{
		close(this->mCGIfd);
		this->mCGIfd = -1;
	}
	this->mServerPort = -1;
	this->mServer = -1;
	this->mRequest.clear();
	this->mResponse.clear();
	this->mAbsolutePath.clear();
	this->mUpload.clear();
	this->mUploadInfo.clear();
	this->mRemain.clear();
	this->mUploadStart.clear();
	this->mUploadEnd.clear();
	this->mUploadStatus = STARTLINE;
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
	if ((now.tv_sec - this->mTime.tv_sec ) + (( now.tv_usec - this->mTime.tv_usec ) / 1000000) > REQ_OVERTIME)
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
	
	std::string boundary = type.substr(pos + 1);
	pos = boundary.find('=');
	if (pos == std::string::npos)
		throw ConnectionException("Boundary not matched format", BAD_REQUEST);
	
	this->mUploadStart = "--" + boundary.substr(pos + 1);
	this->mUploadEnd = this->mUploadStart + "--";
	
	boundary.erase(pos);
	if (ft::trim(boundary) != "boundary")
		throw ConnectionException("Boundary not matched format", BAD_REQUEST);

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
	if (this->mRequest.getStatus() == CHECK_TYPE
			&& status == BODY)
		this->mRequest.setStatus(status);
}

void Connection::setAbsolutePath(std::string const & root, std::string const & url, std::string const & type)
{
	std::string path = root;

	if (*path.rbegin() == '/')
		path.erase(path.size() - 1);

	if (*url.begin() != '/')
		path += "/" + url;
	else
		path += url;

	this->mAbsolutePath = path;
	this->mResponse.setContentType(type);
}

void Connection::setUpload(void)
{
	while (!this->mRemain.empty()
			&& this->mRemain.end() != std::find(this->mRemain.begin(), this->mRemain.end(), '\n')
			&& this->mRequest.getStatus() != COMPLETE)
	{
		std::vector<char> line;
		while (this->mRemain.front() != '\n')
		{
			line.push_back(this->mRemain.front());
			this->mRemain.pop_front();
		}
		line.push_back(this->mRemain.front());
		this->mRemain.pop_front();

		std::string str;
		for (size_t i = 0; i < line.size(); i++)
			str.push_back(line[i]);

		int content_length = this->mRequest.getContentLength();
		if (this->mRequest.isChunk())
		{
			if (line[0] == '\r' && line[1] == '\n')
				continue ;

			if (content_length <= 0)
			{
				try
				{
					content_length = ft::toInt(str, 16);
				}
				catch (int e)
				{
					throw ConnectionException("Transfer Chunk Content-length is not number", BAD_REQUEST);
				}
				if (content_length == 0)
					this->mRequest.setStatus(COMPLETE);
				this->mRequest.setContentLength(content_length);
			}
			else
			{
				this->setUploadData(str, line);
				content_length -= line.size() - 2;
				this->mRequest.setContentLength(content_length);
			}
		}
		else
		{
			int length = line.size();
			if (length < content_length)
			{
				content_length -= length;
				this->mRequest.setContentLength(content_length);
				this->setUploadData(str, line);
			}
			else
			{
				line.resize(content_length);
				str.resize(content_length);
				this->mRequest.setContentLength(0);
				this->setUploadData(str, line);
			}
		}
	}
}

void Connection::setUploadData(std::string & st, std::vector<char> & vt)
{
	ft::trim(st);
	if (this->mUploadStatus == STARTLINE
			&& !st.empty()
			&& st == this->mUploadStart)
	{
		this->mUpload.push_back(std::vector<char>());
		this->mUploadInfo.push_back(std::vector<std::string>());
		this->mUploadStatus = HEADER;
	}
	else if (this->mUploadStatus == BODY
			&& !st.empty()
			&& st == this->mUploadEnd)
	{
		this->mUploadStatus = COMPLETE;
		this->mRequest.setStatus(COMPLETE);
	}
	else if (this->mUploadStatus == BODY
			&& !st.empty()
			&& st == this->mUploadStart)
	{
		this->mUpload.push_back(std::vector<char>());
		this->mUploadInfo.push_back(std::vector<std::string>());
		this->mUploadStatus = HEADER;
	}
	else if (this->mUploadStatus == HEADER
			&& !st.empty())
		this->mUploadInfo.back().push_back(st);
	else if (this->mUploadStatus == HEADER
			&& st.empty())
		this->mUploadStatus = BODY;
	else if (this->mUploadStatus == BODY)
	{
		for (size_t i = 0; i < vt.size(); i++)
			this->mUpload.back().push_back(vt[i]);
	}
	else
		throw ConnectionException("Upload Body not match format", BAD_REQUEST);
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

eStatus Connection::getStatus(void) const
{
	return this->mStatus;
}

void Connection::setCGI(std::string const & cgi)
{
	this->mCGI = cgi;
}

bool Connection::checkReadDone(void)
{
	return (this->mStatus == COMPLETE && this->mRequest.getStatus() == COMPLETE);
}

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
		case CGI_COMPLETE:
			std::cout << "CGI_COMPLETE";
			break ;
		case COMPLETE:
			std::cout << "COMPLETE";
			break ;
		default:
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
			std::cout << "\t\t";
			for (size_t j = 0; j < this->mUpload[i].size() && j < 10; j++)
			{
				std::cout << this->mUpload[i][j];
			}
			std::cout << std::endl;
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
	envp["QUERY_STRING"] = this->mRequest.getQuery();
	envp["REMOTE_ADDR"] = "";
	envp["REMOTE_HOST"] = "";
	envp["REMOTE_IDENT"] = "";
	envp["REMOTE_USER"] = "";
	envp["REDIRECT_STATUS"] = "200";
	envp["SCRIPT_NAME"] = this->mCGI;
	envp["SCRIPT_FILENAME"] = this->mCGI;
	envp["SERVER_NAME"] = this->mRequest.findHeader("Host");
	envp["GATEWAY_INTERFACE"] = "CGI/1.1";
	envp["SERVER_PROTOCOL"] = "HTTP/1.1";
	envp["SERVER_SOFTWARE"] = "webserv/1.0";
	envp["PATH_TRANSLATED"] = this->mAbsolutePath;

	switch(this->mRequest.getMethod())
	{
		case GET :
			envp["REQUEST_METHOD"] = "GET";
			break;
		case POST :
			envp["CONTENT_LENGTH"] = ft::toString(this->mRequest.getBodySize(), 10);
			envp["REQUEST_METHOD"] = "POST";
			envp["CONTENT_TYPE"] = this->mRequest.findHeader("Content-Type");
			break;
		case DELETE :
			envp["REQUEST_METHOD"] = "DELETE";
			break;
		default:
			envp["REQEUST_METHOD"] = "UNKNOWN";
			break;
	}

	envp["PATH_INFO"] = this->mRequest.getUrl();
	envp["REQUEST_URI"] = this->mRequest.getUrl() + this->mRequest.getQuery();
}
