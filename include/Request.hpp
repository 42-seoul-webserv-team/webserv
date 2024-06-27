#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>
# include <map>
# include "enum.hpp"
# include "ft.hpp"

# define HEADER_LENGTH_MAX 1000000
# define URL_LENGTH_MAX 1000000

class Request
{
	private:

		eStatus mReadStatus;
		eMethod mMethod;
		std::string mUrl;
		std::string mQuery;
		std::string mBody;
		std::string mContentType;
		std::map<std::string, std::string> mHeader;
		
		bool mContentChunk;
		int mContentLength;
		size_t mHeaderLength;

		void setStartLine(std::string const & line);
		void setHeader(std::string const & line);
		void setBody(std::string const & line);

	public:
		Request(void);
		~Request(void);

		eMethod getMethod(void) const;
		std::string getUrl(void) const;
		eStatus getStatus(void) const;
		std::string getBody(void) const;

		std::string findHeader(std::string const & key);
		void set(std::string const & line);

		bool checkBodyComplete(void);

		void printAll(void);
};

#endif
