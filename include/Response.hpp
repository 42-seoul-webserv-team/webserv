#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <string>

# include "ft.hpp"

class Response
{
	private:
		std::string mCode;
		std::string mStatusMsg;
		std::string mContentType;
		std::string mContentLength;
		std::string mServerName;
		std::string mBody;

	public:
		Response(void);
		~Response(void);

		void clear(void);

		std::string getCode(void);
		std::string getStatusMsg(void);
		std::string getContentType(void);
		std::string getContentLength(void);
		std::string getServerName(void);
		std::string getBody(void);

		void setCode(unsigned int code);
		void setStatusMsg(std::string const & msg);
		void setContentType(std::string const & type);
		void setServerName(std::string const & server);
		void setBody(std::string const & body);

		void printAll(void);
};

#endif
