#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <string>
# include "ft.hpp"

# define DEFAULT_ERR_CODE "WEBSERV_STATUS_CODE"
# define DEFAULT_ERR_MSG "WEBSERV_STATUS_MSG"
# define DEFAULT_ERR_SERVERNAME "WEBSERV_SERVER_NAME"

enum eResAccessStatus
{
	READY,
	FINISH,
};

class Response
{
	private:
		eResAccessStatus mAccessStatus;
		std::string mCode;
		std::string mStatusMsg;
		bool mConnectionHeader;
		std::string mContentType;
		std::string mContentLength;
		std::string mRedirection;
		std::string mServerName;
		std::string mBody;

	public:
		Response(void);
		~Response(void);
		eResAccessStatus getAccessStatus(void);
		std::string getCode(void);
		std::string getStatusMsg(void);
		bool getConnectionHeader(void);
		std::string getContentType(void);
		std::string getContentLength(void);
		std::string getRedirection(void);
		std::string getServerName(void);
		std::string getBody(void);

		void nextStatus(void);
		void setCode(unsigned int code);
		void setStatusMsg(std::string const & msg);
		void setConnectionHeader(void);
		void setContentType(std::string const & type);
		void setRedirection(std::string const & path);
		void setServerName(std::string const & server);
		void setBody(std::string const & body);
};

#endif
