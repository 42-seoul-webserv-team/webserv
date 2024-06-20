#ifndef REQUEST_HPP
# define REQUEST_HPP

// juhylee
# include "status.hpp"
# include <map>
# include <vector>
# include <string>

enum eMethod
{
	UNKNOWN,
	GET,
	POST,
	DELETE,
};

class Request
{
	private:
		eMethod mType;
		std::vector<std::string> mStartLine;
		std::map<std::string, std::string> mHeader;
		std::string mBody;
		std::string mContentType;
		long mContentLength;
		eStatus mReadStatus;

	public:
		Request(void);
		~Request(void);

		std::string findHeader(std::string const & key);
		void set(std::string const & line);
};

#endif
