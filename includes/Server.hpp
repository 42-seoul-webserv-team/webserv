#ifndef SERVER_HPP
# define SERVER_HPP

# include <string>
# include <vector>
# include <fstream>
# include <iostream>
# include "ft.hpp"
# include "Location.hpp"
# include "Response.hpp"

# define DEFAULT_SERVERNAME "WebServ"
# define DEFAULT_BODY_SIZE "4K"

class Server
{
	private:
		int mPort;
		int mSocket;
		int mBodySize;
		std::string mHost;
		std::string mServerName;
		Response mErrorPage;
		std::string mErrorPage;
		std::string mErrorPageType;
		std::vector<Location> mLocation;
		
		std::string getDefaultError(int errocode, std::string const & errmsg);

	public:
		Server(void);
		~Server(void);

		void printAll(void);

		int	getPort(void);
		int	getSocket(void);
		std::string getHost(void);
		std::string getServerName(void);
		Location *getLocation(size_t idx);
		size_t	getLocationSize(void);
		Response getErrorPage(int errcode, std::String const & errmsg);

		void setPort(int port);
		void setSocket(int socket);
		void setBodySize(std::string const & dataSize);
		void setHost(std::string const & host);
		void setServerName(std::string const & servername);
		void setErrorPage(std::string const & file, std::string const & type);
		void addEmptyLocation(void);
};

#endif
