#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <string>
# include "ft.hpp"

enum eResponseStatus
{
	READY,
	RUN,
	COMPLETE,
};

class Response
{
	private:
		std::string mCode;
		std::string mStatusMsg;
		bool mConnectionHeader;
		std::string mContentType;
		std::string mContentLength;
		std::string mRedirection;
		std::string mServerName;
		std::string mBody;
		eResponseStatus mStatus;

	public:
		Response(void);
		~Response(void);
		std::string getCode(void);
		std::string getStatusMsg(void);
		bool getConnectionHeader(void);
		std::string getContentType(void);
		std::string getContentLength(void);
		std::string getRedirection(void);
		std::string getServerName(void);
		std::string getBody(void);
		eResponseStatus getStatus(void);

		void setCode(unsigned int code);
		void setStatusMsg(std::string const & msg);
		void setConnectionHeader(void);
		void setContentType(std::string const & type);
		void setRedirection(std::string const & path);
		void setServerName(std::string const & server);
		void setBody(std::string const & body);
};

#endif
