#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>
# include <map>
# include "ft.hpp"

# define HEADER_LENGTH_MAX 1000000
# define URL_LENGTH_MAX 1000000

enum eStatus
{
	STARTLINE,
	HEADER,
	BODY,
	COMPLETE,
};

class Request
{
	private:
		std::string mMethod;
		std::string mUrl;
		std::string mQuery;
		std::map<std::string, std::string> mHeader;
		std::string mBody;
		eStatus mReadStatus;
		std::string mContentType;
		
		bool mContentChunk;
		int mContentLength;
		size_t mHeaderLength;

		void setStartLine(std::string const & line);
		void setHeader(std::string const & line);
		void setBody(std::string const & line);

	public:
		Request(void);
		~Request(void);

		std::string getMethod(void);
		std::string getUrl(void);
		eStatus getStatus(void);
		std::string getContentType(void);
		std::string getBody(void);

		std::string findHeader(std::string const & key);
		void set(std::string const & line);
		void setContentType(std::string const & type);

		void printAll(void);
};

#endif
