#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <iostream>

# include <vector>
# include <map>
# include <fstream>
# include <exception>
# include <sys/socket.h>
# include <netinet/in.h>
# include <fcntl.h>
# include <dirent.h>
# include "ft.hpp"
# include "Configure.hpp"
# include "Server.hpp"
# include "Connection.hpp"
# include "Kqueue.hpp"
# include "HTTPSender.hpp" // juhyelee - for send error page
# include "Logger.hpp"
# include "Message.hpp"
# include "value.hpp"

// juhyelee - Exception
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
		std::vector<Connection> mConnection;
		Kqueue mKqueue;
		HTTPSender mSender;
		Logger mLogger;

		void validConfig(std::string contents);
		void parseConfig(std::string & contents);
		void createResponseCodeMSG(void);
		void createMIMEType(void);
		void listenServer(void);
		std::string findMIMEType(std::string const & file);
		int findServer(int socket);
		int findServer(Connection &clt);
		std::vector<std::string> parseUrl(std::string const & url);
		void closeConnection(Connection *clt);
		void parseRequest(Connection *clt, Server *svr);

		// juhyelee
		std::map<std::string, std::string> mEnvp;

	public:
		WebServ(void);
		~WebServ(void);

		void configure(std::string const & config);
		void activate();

		void printAll(void);
		// juhyelee - run
		void run(Connection * clt);
		void runGET(Connection * clt);
		void runPOST(Connection * clt);
		void runDELETE(Connection * clt);
		void getFileList(std::vector<std::string> & list, DIR * dir);
		void setEnv(char **&envp);
};

#endif
