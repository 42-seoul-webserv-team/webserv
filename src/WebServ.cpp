//Webserv
#include "WebServ.hpp"
#include "Connection.hpp"
#include <cstdio>
#include <dirent.h>
#include <signal.h>

void WebServ::run(Connection * clt)
{
	if (clt->getReadStatus() < COMPLETE)
	{
		return ;
	}

	switch(clt->getMethod())
	{
		case GET:
			runGET(clt);
			break;
		case POST:
			runPOST(clt);
			break;
		case DELETE:
			runDELETE(clt);
			break;
		default:
			break; 
	}
	clt->setStatus(COMPLETE);
}

void WebServ::runGET(Connection * clt)
{
	eProcessType procType = clt->getProcType();
	if (procType == FILES)
	{
		clt->fillRequest();
	}
	else if (procType == AUTOINDEX)
	{
		DIR * dir = opendir(clt->getAbsolutePath());
		if (dir == NULL)
		{
			clt->fillRequest();
		}
		else
		{
			std::vector<std::string> fileList;
			getFileList(fileList, dir);
			clt->fillRequest(fileList);
			closedir(dir);
		}
	}
	else if (procType == CGI)
	{
		if (clt->getStatus() != PROC_CGI)
		{
			clt->processCGI(this->mKqueue, this->mEnvp);
		}
		else if (!clt->isTimeOver())
		{
			clt->fillRequestCGI();
		}
		else
		{
			kill(clt->getCGIproc(), SIGKILL);
		}
	}
}

void WebServ::runPOST(Connection * clt)
{
	eProcessType procType = clt->getProcType();
	if (procType == FILES)
	{
		DIR * dir = opendir(clt->getAbsolutePath());
		if (dir == NULL)
		{
			throw std::exception(); // 404
		}
		closedir(dir);

		std::string fileName = clt->getAbsolutePath();
		fileName += ".";
		fileName += clt->getContentType(); // 확장자
		std::ofstream file;
		file.open(fileName.c_str());
		if (!file.is_open())
		{
			throw std::exception(); // 500
		}
		file << clt->getReqBody();
		file.close();
	}
	else if (procType == CGI)
	{
		if (clt->getStatus() != PROC_CGI)
		{
			clt->processCGI(this->mKqueue, this->mEnvp);
		}
		else if (!clt->isTimeOver())
		{
			clt->fillRequestCGI();
		}
		else
		{
			kill(clt->getCGIproc(), SIGKILL);
		}
	}
}

void WebServ::runDELETE(Connection * clt)
{
	eProcessType procType = clt->getProcType();
	if (procType == FILES)
	{
		clt->removeFile();
	}
	else if (procType == AUTOINDEX)
	{
		DIR * dir = opendir(clt->getAbsolutePath());
		if (dir == NULL)
		{
			clt->removeFile();
		}
		else
		{
			struct dirent * file = readdir(dir); // 첫 . 는 버린다.
			file = readdir(dir); // 두번째 .. 도 버린다.
			while ((file = readdir(dir)) != NULL)
			{
				if (std::remove(file->d_name) < 0)
				{
					throw std::exception(); // 500
				}
			}
			closedir(dir);
		}
	}
	else if (procType == CGI)
	{
		if (clt->getStatus() != PROC_CGI)
		{
			clt->processCGI(this->mKqueue, this->mEnvp);
		}
		else if (!clt->isTimeOver())
		{
			clt->fillRequestCGI();
		}
		else
		{
			kill(clt->getCGIproc(), SIGKILL);
		}
	}
}

void WebServ::getFileList(std::vector<std::string> & list, DIR * dir)
{
	struct dirent * file = readdir(dir); // 첫 . 는 버린다.
	while ((file = readdir(dir)) != NULL)
	{
		std::string str = file->d_name;
		list.push_back(str);
	}
}

void WebServ::setEnv(char **&envp)
{
	while (*envp != NULL)
	{
		std::string str = *envp;
		size_t pos = str.find('=');
		if (pos != std::string::npos)
		{
			std::string key = str.substr(0, pos);
			std::string value = str.substr(pos + 1);
			this->mEnvp[key] = value;
		}
		envp++;
	}
}

WebServ::WebServ(void)
{
	this->createResponseCodeMSG();
	this->createMIMEType();
}

WebServ::~WebServ(void)
{
	this->mServers.clear();
	this->mPortGroup.clear();
	this->mResponseCodeMSG.clear();
	this->mMIMEType.clear();
}

std::string WebServ::findMIMEType(std::string const & file) 
{
	size_t		pos = file.rfind('.');

	if (pos == std::string::npos)
		return "application/octet-stream";

	std::string extension = file.substr(pos);
	std::map<std::string, std::string>::iterator it = this->mMIMEType.find(extension);

	if (it == this->mMIMEType.end()
			|| it->first.size() == file.size())
		return "application/octet-stream";

	return it->second;
}

void WebServ::createResponseCodeMSG(void)
{
	this->mResponseCodeMSG[301] = "Moved Permanently";
	this->mResponseCodeMSG[400] = "Bad Request";
	this->mResponseCodeMSG[403] = "Forbidden";
	this->mResponseCodeMSG[404] = "Not Found";
	this->mResponseCodeMSG[405] = "Method not allowed";
	this->mResponseCodeMSG[408] = "Request timeout";
	this->mResponseCodeMSG[411] = "Length Required";
	this->mResponseCodeMSG[413] = "Request entity too large";
	this->mResponseCodeMSG[414] = "Request-URI too long";
	this->mResponseCodeMSG[415] = "Unsupported media type";	
	this->mResponseCodeMSG[500] = "Internal Server Error";
	this->mResponseCodeMSG[505] = "HTTP Version Not Supported";
}

void WebServ::createMIMEType(void)
{
	this->mMIMEType[".aac"] = "audio/aac"; 
	this->mMIMEType[".abw"] = "application/x-abiword"; 
	this->mMIMEType[".apng"] = "image/apng";
	this->mMIMEType[".arc"] = "application/x-freearc";
	this->mMIMEType[".avif"] = "image/avif";
	this->mMIMEType[".avi"] = "Avideo/x-msvideo";
	this->mMIMEType[".azw"] = "application/vnd.amazon.ebook";
	this->mMIMEType[".bin"] = "application/octet-stream";
	this->mMIMEType[".bmp"] = "image/bmp";
	this->mMIMEType[".bz"] = "application/x-bzip";
	this->mMIMEType[".bz2"] = "application/x-bzip2";
	this->mMIMEType[".cda"] = "application/x-cdf";
	this->mMIMEType[".csh"] = "application/x-csh";
	this->mMIMEType[".css"] = "text/css";
	this->mMIMEType[".csv"] = "text/csv";
	this->mMIMEType[".doc"] = "application/msword";
	this->mMIMEType[".docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
	this->mMIMEType[".eot"] = "application/vnd.ms-fontobject";
	this->mMIMEType[".epub"] = "application/epub+zip";
	this->mMIMEType[".gz"] = "application/gzip";
	this->mMIMEType[".gif"] = "image/gif";
	this->mMIMEType[".htm"] = "text/html; charset=utf-8";
	this->mMIMEType[".html"] = "text/html; charset=utf-8";
	this->mMIMEType[".ico"] = "image/vnd.microsoft.icon";
	this->mMIMEType[".ics"] = "text/calendar";
	this->mMIMEType[".jar"] = "application/java-archive";
	this->mMIMEType[".jpeg"] = "image/jpeg";
	this->mMIMEType[".jpg"] = "image/jpeg";
	this->mMIMEType[".js"] = "text/javascript"; 
	this->mMIMEType[".json"] = "application/json"; 
	this->mMIMEType[".jsonld"] = "application/ld+json";
	this->mMIMEType[".mid"] = "audio/midi";
	this->mMIMEType[".midi"] = "audio/midi";
	this->mMIMEType[".mjs"] = "text/javascript";
	this->mMIMEType[".mp3"] = "audio/mpeg";
	this->mMIMEType[".mp4"] = "video/mp4";
	this->mMIMEType[".mpeg"] = "video/mpeg";
	this->mMIMEType[".mpkg"] = "application/vnd.apple.installer+xml";
	this->mMIMEType[".odp"] = "application/vnd.oasis.opendocument.presentation";
	this->mMIMEType[".ods"] = "application/vnd.oasis.opendocument.spreadsheet";
	this->mMIMEType[".odt"] = "application/vnd.oasis.opendocument.text";
	this->mMIMEType[".oga"] = "audio/ogg";
	this->mMIMEType[".ogv"] = "video/ogg";
	this->mMIMEType[".ogx"] = "application/ogg";
	this->mMIMEType[".opus"] = "audio/opus";
	this->mMIMEType[".otf"] = "font/otf";
	this->mMIMEType[".png"] = "image/png";
	this->mMIMEType[".pdf"] = "application/pdf";
	this->mMIMEType[".php"] = "application/x-httpd-php";
	this->mMIMEType[".ppt"] = "application/vnd.ms-powerpoint";
	this->mMIMEType[".pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
	this->mMIMEType[".rar"] = "application/vnd.rar";
	this->mMIMEType[".rtf"] = "application/rtf";
	this->mMIMEType[".sh"] = "application/x-sh";
	this->mMIMEType[".svg"] = "image/svg+xml";
	this->mMIMEType[".tar"] = "application/x-tar";
	this->mMIMEType[".tif"] = "image/tiff";
	this->mMIMEType[".tiff"] = "image/tiff";
	this->mMIMEType[".ts"] = "video/mp2t";
	this->mMIMEType[".ttf"] = "font/ttf";
	this->mMIMEType[".txt"] = "text/plain";
	this->mMIMEType[".vsd"] = "application/vnd.visio";
	this->mMIMEType[".wav"] = "audio/wav";
	this->mMIMEType[".weba"] = "audio/webm";
	this->mMIMEType[".webm"] = "video/webm";
	this->mMIMEType[".webp"] = "image/webp";
	this->mMIMEType[".woff"] = "font/woff";
	this->mMIMEType[".woff2"] = "font/woff2";
	this->mMIMEType[".xhtml"] = "application/xhtml+xml";
	this->mMIMEType[".xls"] = "application/vnd.ms-excel";
	this->mMIMEType[".xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	this->mMIMEType[".xml"] = "application/xml";
	this->mMIMEType[".xul"] = "application/vnd.mozilla.xul+xml";
	this->mMIMEType[".zip"] = "application/zip";
	this->mMIMEType[".3gp"] = "video/3gpp";
	this->mMIMEType[".3g2"] = "video/3gpp2";
	this->mMIMEType[".7z"] = "application/x-7z-compressed";
}

void WebServ::configure(std::string const & config)
{
	std::ifstream	conf;
	std::string		contents;

	conf.open(config.c_str());
	if (!conf.is_open())
		throw ManagerException("Configuration open() failed!");

	while (conf.good())
		contents.push_back(conf.get());
	contents.pop_back(); 
	
	if (conf.bad())
	{
		conf.close();
		throw ManagerException("Configuration read() failed!");
	}

	conf.close();

	try { 
		this->validConfig(contents); 
	} catch (size_t row) {
		throw ManagerException("Configuration " + config + ":" + ft::toString(row, 10) + " not valid!");
	}

	this->parseConfig(contents);
	this->listenServer();
	this->mLogger.open();
	this->printAll();
}

void WebServ::listenServer(void)
{
	for (size_t i = 0; i < this->mServers.size(); i++)
	{
		Server &svr = this->mServers[i];
		int port = svr.getPort();
		std::map<int, std::vector<int> >::iterator group = this->mPortGroup.find(port);
		if (group == this->mPortGroup.end())
		{
			this->mPortGroup[port] = std::vector<int>(1, i);
			int svr_socket = socket(PF_INET, SOCK_STREAM, 0);
			if (svr_socket == -1)
				throw ManagerException("Server socket() failed!");

			svr.setSocket(svr_socket);

			struct sockaddr_in addr;
			addr.sin_family = PF_INET;
			addr.sin_port = htons(port);
			addr.sin_addr.s_addr = htonl(INADDR_ANY);
			if (bind(svr_socket, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr)) == -1)
				throw ManagerException("Server bind() failed!");

			if (listen(svr_socket, LISTEN_MAX) == -1)
				throw ManagerException("Server listen() failed!");

			if (fcntl(svr_socket, F_SETFL, O_NONBLOCK) == -1)
				throw ManagerException("Server fcntl() failed!");

			this->mKqueue.addEvent(svr_socket, nullptr);
		}
		else
			group->second.push_back(i);
	}
}

Server *WebServ::findServer(int socket)
{
	for (size_t i = 0; i < this->mServers.size(); i++)
	{
		if (this->mServers[i].getSocket() == socket)
			return &(this->mServers[i]);
	}
	return nullptr;
}

Server *WebServ::findServer(Connection *clt)
{
	Server *svr = clt->getServer();
	if (svr != nullptr)
		return svr;
	
	std::string host = clt->getHost();
	if (host.empty())
		return nullptr;
	
	std::map<int, std::vector<int> >::iterator it = this->mPortGroup.find(clt->getPort());
	if (it == this->mPortGroup.end())
		return nullptr;

	for (size_t i = 0; i < it->second.size(); i++)
	{
		int idx = it->second[i];
		if (this->mServers[idx].getHost() == host)
		{
			clt->setServer(&(this->mServers[idx]));
			return clt->getServer();
		}
	}
	
	clt->setServer(&(this->mServers[it->second.front()]));
	return clt->getServer();
}

void WebServ::closeConnection(Connection *clt)
{
	int socket = clt->getSocket();
	clt->closeSocket();
	std::vector<Connection>::iterator it = this->mConnection.begin();
	while (it != this->mConnection.end())
	{
		if (it->getSocket() == socket)
		{
			this->mConnection.erase(it);
			return ;
		}
	}
}

void WebServ::activate()
{
	if (this->mEnvp.empty())
		return ;

	this->mKqueue.checkEvent();

	while (true)
	{
		// juhyelee - for send Error page
		Server *svr = NULL;

		struct kevent *curEvent = this->mKqueue.getEvent();
		if (curEvent == nullptr)
			return ;
		if (curEvent->udata == nullptr
				&& (curEvent->flags & EV_ERROR))
			throw ManagerException("Server socket failed");
		try 
		{
			if (curEvent->udata == nullptr
					&& (curEvent->flags & EVFILT_READ))
			{
				this->mLogger.putAccess("connect connection");
				svr = this->findServer(curEvent->ident); // juhyelee - for send Error page
				if (svr == nullptr)
					throw ManagerException("Cannot find Server");
				int clt_socket = accept(svr->getSocket(), NULL, NULL);
				if (clt_socket == -1)
					throw ManagerException("accpet connection failed");

				if (fcntl(clt_socket, F_SETFL, O_NONBLOCK) == -1)
					throw ManagerException("fcntl connection socket fialed");

				this->mConnection.push_back(Connection(clt_socket, svr->getPort()));
				this->mKqueue.addEvent(clt_socket, &(this->mConnection.back()));
			}
			if (curEvent->udata != nullptr
					&& (curEvent->flags & EV_ERROR))
				throw ManagerException("Connection socket failed");

			if (curEvent->udata != nullptr
					&& (curEvent->flags & EVFILT_READ))
			{
				Connection *clt = static_cast<Connection *>(curEvent->udata);
				clt->readRequest();
				Server *svr = this->findServer(clt);
				if (svr != nullptr)
					this->parseRequest(clt, svr);
			}
			if (curEvent->udata != nullptr
					&& (curEvent->flags & EVFILT_WRITE))
			{
				Connection *clt = static_cast<Connection *>(curEvent->udata);
				clt->printAll();
				this->closeConnection(clt);
				this->run(clt);
				/*clt->writeResponse();
				if (clt->checkComplete())
				{
					this->mSender.send(clt->getResponse());
					this->mLogger.putAccess("");
					this->closeConnection(clt);
				}*/
			}
		} 
		/*
		catch (redirectionException & e)
		{
			this->mSender.send();
			this->closeConnection(curEvent->udata);
			this->mLogger.putAccess("");
		}
		catch (connectionException & e)
		{
			Server *svr = this->findServer(curEvent->udata);
			this->mSender.send(svr->getErrorPage(e.getCode(), this->mResponseCodeMSG[e.getCode()]));
			this->mLogger.putError("");
			this->closeConnection(curEvent->udata);
			this->mLogger.putAccess("");
		}
		*/
		catch (ConnectionException & e)
		{
			this->mLogger.putError(e.what());
			Connection *clt = static_cast<Connection *>(curEvent->udata); // juhyelee - for send error page
			if (clt != NULL)
			{
				// juhyelee - Send Error Page
				Response errorResponse = svr->getErrorPage(e.getErrorCode(), e.what());
				HTTPSender sender;
				sender.sendMessage(clt->getSocket, errorResponse);
				this->closeConnection(clt);
				this->mLogger.putAccess("close connection");
			}
		}
		catch (RedirectionException & e) // juhyelee - Send redirection
		{
			this->mLogger.putError(e.what());
			Connection *clt = static_cast<Connection *>(curEvent->udata);
			if (clt != NULL)
			{
				HTTPSender sender;
				sender.sendMessage(clt->getSocket, e.getRedirLoc(), e.getServerName());
				this->closeConnection(clt);
				this->mLogger.putAccess("redirection");
				this->mLogger.putAccess("close connection");
			}
		}
	}

	/*std::vector<Connection>::iterator it = this->mConnection.begin();
	while (it != this->mConnection.end())
	{
		if (it->checkOvertime())
		{
			Server *svr = this->findServer(curEvent->udata);
			this->mSender.send(svr->getErrorPage(408, this->mResponseCodeMSG[408]));
			this->mLogger.putAccess();
			it->close();
			it = this->mConnection.erase(it);
		}
		else
			it++;
	}*/
}

void WebServ::parseRequest(Connection *clt, Server *svr)
{
	if (clt->getStatus() == COMPLETE || !clt->checkStatus())
		return ;

	if (clt->getStatus() == STARTLINE && clt->checkStatus())
	{
		std::vector<std::string> url = this->parseUrl(clt->getUrl());
		Location *lct = svr->findLocation(url);
		if (lct == nullptr)
			throw ConnectionException("Can't find Location", NOT_FOUND); // connectionException(404);

		if (!lct->checkMethod(clt->getMethod()))
			throw ConnectionException("Not allowed Method", MATHOD_NOT_ALLOWED); // connectionException(405);

		if (!lct->getRedirect().empty())
			throw RedirectionException(lct->getRedirect(), svr->getServerName()); // redirectionException(lct->getRedirect());
		else if (lct->checkIndexFile(url))
		{
			clt->setAbsolutePath(lct->getRoot(), lct->getIndex(), this->findMIMEType(lct->getIndex()));
			if (lct->checkCGI(clt->getAbsolutePath()))
			{
				clt->setType(CGI);
				clt->setCGI(lct->getCGI(clt->getAbsolutePath()));
			}
			else
				clt->setType(FILES);
		}
		else if (lct->checkAutoindex())
		{
			clt->setAbsolutePath(lct->getRoot(), clt->getUrl(), "text/html");
			clt->setType(AUTOINDEX);
		}
		else if (lct->checkCGI(clt->getUrl()))
		{
			clt->setAbsolutePath(lct->getRoot(), clt->getUrl(), this->findMIMEType(clt->getUrl()));
			clt->setType(CGI);
			clt->setCGI(lct->getCGI(clt->getUrl()));
		}
		else
		{
			clt->setAbsolutePath(lct->getRoot(), clt->getUrl(), this->findMIMEType(clt->getUrl()));
			clt->setType(FILES);
		}
		clt->setContentType(this->findMIMEType(clt->getAbsolutePath()));
		clt->setStatus(HEADER);
	}

	if (clt->getStatus() == HEADER && clt->checkStatus())
	{
		eMethod  method = clt->getMethod();
		if (method == GET || method == DELETE)
			clt->setStatus(COMPLETE);
		else if (method == POST)
		{
			if (clt->checkUpload())
			{
				std::vector<std::string> url = this->parseUrl(clt->getUrl());
				Location *lct = svr->findLocation(url);
				if (lct == nullptr)
					throw ConnectionException("Can't find Location", NOT_FOUND); // connectionException(404);

				if (!lct->getUpload().empty())
				{
					clt->setAbsolutePath(lct->getRoot(), lct->getUpload(), "text/html");
					clt->setAbsolutePath(clt->getAbsolutePath(), clt->getUrl(), "text/html");
				}
				clt->setType(UPLOAD);
			}
			clt->setStatus(BODY);
		}
	}

	if (clt->getStatus() == BODY && clt->checkStatus())
	{
		if (clt->getType() == UPLOAD)
			clt->setUpload();
		clt->setStatus(COMPLETE);
	}
}

void WebServ::validConfig(std::string contents)
{
	size_t	pos = 0;
	size_t	rows = 0;
	bool	serverFlag = false;
	bool	locationFlag = false;
	bool	portFlag = false;
	bool	hostNameFlag = false;
	bool	serverNameFlag = false;
	bool	errPageFlag = false;
	bool	limitBodySizeFlag = false;
	bool	allowMethodFlag = false;
	bool	rootPathFlag = false;
	bool	indexFileFlag = false;
	bool	autoindexFlag = false;
	bool	uploadPathFlag = false;
	bool	redirectFlag = false;

	while (!contents.empty())
	{
		rows++;
		pos = contents.find(NEWLINE);
		if (pos == std::string::npos)
			pos = contents.size();
		
		std::vector<std::string> line = 
			ft::split(contents.substr(0, pos));
		contents.erase(0, pos + 1);
		
		if (line.empty())
			continue;
		else if (line.front() == CONFIG_ACCESS_LOG
				&& serverFlag == false
				&& locationFlag == false
				&& line.size() == 2)
			continue;
		else if (line.front() == CONFIG_ERROR_LOG
				&& serverFlag == false
				&& locationFlag == false
				&& line.size() == 2)
			continue;
		else if (line.front() == SERVER_BLOCK
				&& serverFlag == false
				&& locationFlag == false
				&& line.size() == 2
				&& line.back() == BLOCK_OPEN)
			serverFlag = true;
		else if (line.front() == LISTEN_PORT
				&& serverFlag == true
				&& locationFlag == false
				&& portFlag == false
				&& line.size() == 2)
		{
			try {
				int port = ft::toInt(line.back(), 10);
				if (port < PORT_MIN || port > PORT_MAX)
					throw port;
			} catch (int e) {
				throw rows;
			}
			portFlag = true;
		}
		else if (line.front() == HOST_NAME
				&& serverFlag == true
				&& locationFlag == false
				&& hostNameFlag == false
				&& line.size() == 2)
			hostNameFlag = true;
		else if (line.front() == SERVER_NAME
				&& serverFlag == true
				&& locationFlag == false
				&& serverNameFlag == false
				&& line.size() == 2)
			serverNameFlag = true;
		else if (line.front() == DEFAULT_ERROR
				&& serverFlag == true
				&& locationFlag == false
				&& errPageFlag == false
				&& line.size() == 2)
			errPageFlag = true;
		else if (line.front() == LIMIT_BODY_SIZE
				&& serverFlag == true
				&& locationFlag == false
				&& limitBodySizeFlag == false
				&& line.size() == 2
				&& (line.back().back() != 'M' 
					|| line.back().back() != 'K'))
		{
			std::string bodySize = line.back();
			for (size_t i = 1; i < bodySize.size() - 1; i++)
			{
				if (bodySize[i] < '0' || bodySize[i] > '9')
					throw rows;
			}
			limitBodySizeFlag = true;
		}
		else if (line.front() == LOCATION_BLOCK
				&& serverFlag == true
				&& locationFlag == false
				&& line.size() == 3
				&& line.back() == BLOCK_OPEN)
			locationFlag = true;
		else if (line.front() == ALLOW_METHODS
				&& locationFlag == true
				&& allowMethodFlag == false)
		{
			for (size_t i = 1; i < line.size(); i++)
			{
				std::string method = line[i];
				if (method != "GET"
						&& method != "POST"
						&& method != "DELETE")
					throw rows;
			}
			allowMethodFlag = true;
		}
		else if (line.front() == ROOT_PATH
				&& locationFlag == true
				&& rootPathFlag == false
				&& line.size() == 2)
			rootPathFlag = true;
		else if (line.front() == INDEX_FILE
				&& locationFlag == true
				&& indexFileFlag == false
				&& line.size() == 2)
			indexFileFlag = true;
		else if (line.front() == AUTOINDEX_FLAG
				&& locationFlag == true
				&& autoindexFlag == false
				&& line.size() == 2
				&& (line.back() == AUTOINDEX_ON
					|| line.back() == AUTOINDEX_OFF))
		{
			if (line.back() == AUTOINDEX_ON)
				autoindexFlag = true;
		}
		else if (line.front() == CGI_CONFIG
				&& locationFlag == true
				&& line.size() == 3)
			continue;
		else if (line.front() == UPLOAD_PATH
				&& locationFlag == true
				&& uploadPathFlag == false
				&& line.size() == 2)
			uploadPathFlag = true;
		else if (line.front() == REDIRECTION
				&& locationFlag == true
				&& redirectFlag == false
				&& line.size() == 2)
			redirectFlag = true;
		else if (line.front() == BLOCK_CLOSE
				&& serverFlag == true
				&& locationFlag == false
				&& line.size() == 1
				&& portFlag == true
				&& hostNameFlag == true)
		{
			serverFlag = false;
			portFlag = false;
			hostNameFlag = false;
			serverNameFlag = false;
			errPageFlag = false;
		}
		else if (line.front() == BLOCK_CLOSE
				&& locationFlag == true
				&& line.size() == 1
				&& rootPathFlag == true
				&& (indexFileFlag == true
					|| autoindexFlag == true))
		{
			allowMethodFlag = false;
			rootPathFlag = false;
			indexFileFlag = false;
			autoindexFlag = false;
			uploadPathFlag = false;
			redirectFlag = false;
			locationFlag = false;
		}
		else
			throw rows;
	}
}

std::vector<std::string> WebServ::parseUrl(std::string const & url)
{
	size_t idx = 0;
	std::vector<std::string> ret;
	while (url[idx])
	{
		std::string path;
		while (url[idx] && url[idx] != '/')
		{
			path.push_back(url[idx]);
			idx++;
		}
		ret.push_back(path);
		if (url[idx] == '/')
			idx++;
	}
	return ret;
}

void	WebServ::parseConfig(std::string & contents)
{
	size_t		pos = 0;
	Server		*curSvr = NULL;
	Location	*curLct = NULL;
	size_t		locationIdx = 0;
	bool		locationCloseFlag = false;

	while (!contents.empty())
	{
		pos = contents.find(NEWLINE);
		if (pos == std::string::npos)
			pos = contents.size();
		
		std::vector<std::string> line = 
			ft::split(contents.substr(0, pos));
		contents.erase(0, pos + 1);
		if (line.empty())
			continue;
		else if (line.front() == CONFIG_ACCESS_LOG)
			this->mLogger.setAccessLogFile(line.back());
		else if (line.front() == CONFIG_ERROR_LOG)
			this->mLogger.setErrorLogFile(line.back());
		else if (line.front() == SERVER_BLOCK) {
			this->mServers.push_back(Server());
			curSvr = &(this->mServers.back());
		}
		else if (line.front() == LISTEN_PORT)
			curSvr->setPort(ft::toInt(line.back(), 10));
		else if (line.front() == HOST_NAME)
			curSvr->setHost(line.back());
		else if (line.front() == SERVER_NAME)
			curSvr->setServerName(line.back());
		else if (line.front() == DEFAULT_ERROR)
			curSvr->setErrorPage(line.back(), this->findMIMEType(line.back()));
		else if (line.front() == LIMIT_BODY_SIZE)
			curSvr->setBodySize(line.back());
		else if (line.front() == LOCATION_BLOCK)
		{
			locationCloseFlag = true;
			curSvr->addEmptyLocation();
			curLct = curSvr->getLocation(locationIdx);
			std::vector<std::string> path = this->parseUrl(line[1]);
			curLct->setPath(path);
		}
		else if (line.front() == ALLOW_METHODS)
		{
			curLct->clearMethod();
			for (size_t i = 1; i < line.size(); i++)
			{
				if (line[i] == "GET")
					curLct->addMethod(GET);
				else if (line[i] == "POST")
					curLct->addMethod(POST);
				else if (line[i] == "DELETE")
					curLct->addMethod(DELETE);
			}
		}
		else if (line.front() == ROOT_PATH)
			curLct->setRoot(line.back());
		else if (line.front() == INDEX_FILE)
			curLct->setIndex(line.back());
		else if (line.front() == AUTOINDEX_FLAG)
		{
			if (line.back() == AUTOINDEX_ON)
				curLct->setAutoindex(true);
			else if (line.back() == AUTOINDEX_OFF)
				curLct->setAutoindex(false);
		}
		else if (line.front() == CGI_CONFIG)
			curLct->addCgi(line[1], line[2]);
		else if (line.front() == UPLOAD_PATH)
			curLct->setUpload(line.back());
		else if (line.front() == REDIRECTION)
			curLct->setRedirect(line.back());
		else if (line.front() == BLOCK_CLOSE
				&& locationCloseFlag == true)
		{
			locationIdx++;
			locationCloseFlag = false;
		}
		else if (line.front() == BLOCK_CLOSE
				&& locationCloseFlag == false)
			locationIdx = 0;
	}
}

void WebServ::printAll(void)
{
	for(std::vector<Server>::iterator it = this->mServers.begin(); it != this->mServers.end(); it++)
	{
		it->printAll();
		std::cout << std::endl;
	}
}
