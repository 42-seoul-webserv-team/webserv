#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <map>
# include <string>
# include <iostream>

# include "ft.hpp"
# include "enum.hpp"
# include "value.hpp"
# include "ConnectionException.hpp"

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

		void clear(void);

		int getBodySize(void);
		eMethod getMethod(void) const;
		std::string getUrl(void) const;
		eStatus getStatus(void) const;
		void setStatus(eStatus status);
		std::string getBody(void) const;
		std::string findHeader(std::string const & key);
		void set(std::string const & line);
		std::string getQuery(void);
		bool isChunk(void);

		void printAll(void);

		int getContentLength(void) const;
		void setContentLength(int length);
		std::string getContentType(void) const;
};

#endif
