#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <map>
# include <vector>
# include <cstdio>
# include <fstream>
# include <iostream>
# include <exception>

# include <fcntl.h>
# include <signal.h>
# include <dirent.h>
# include <netinet/in.h>
# include <sys/socket.h>

# include "ft.hpp"
# include "enum.hpp"
# include "value.hpp"
# include "Logger.hpp"
# include "Kqueue.hpp"
# include "Server.hpp"
# include "Message.hpp"
# include "Configure.hpp"
# include "Connection.hpp"
# include "HTTPSender.hpp"
# include "ManagerExcption.hpp"
# include "ConnectionException.hpp"
# include "RedirectionException.hpp"

class Connection;
class WebServ {
	private:
		std::vector<Server> mServers;
		std::map<int, std::vector<int> > mPortGroup;
		std::map<int, std::string> mResponseCodeMSG;
		std::map<std::string, std::string> mMIMEType;
		Connection mConnection[EVENT_MAX];
		std::map<std::string, std::string> mEnvp;
		Kqueue mKqueue;
		HTTPSender mSender;
		Logger mLogger;

		std::vector<std::string> parseUrl(std::string const & url);
		std::string findMIMEType(std::string const & file);
		int findServer(int socket);
		int findServer(Connection &clt);
		void parseRequest(Connection *clt, Server *svr);
		void printAll(void);
		void run(Connection * clt);
		void runGET(Connection * clt);
		void runPOST(Connection * clt);
		void runDELETE(Connection * clt);
		void getFileList(std::vector<std::string> & list, DIR * dir);
		void validConfig(std::string contents);
		void parseConfig(std::string & contents);
		void createResponseCodeMSG(void);
		void createMIMEType(void);
		void listenServer(void);

	public:
		WebServ(void);
		~WebServ(void);

		void configure(std::string const & config);
		void activate(void);
		void setEnv(char **&envp);
};

#endif
