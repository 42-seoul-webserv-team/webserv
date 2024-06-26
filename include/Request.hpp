#ifndef REQUEST_HPP
# define REQUEST_HPP

// juhylee
# include "enum.hpp"
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
		
		//juhyelee - need for run
		eStatus getStatus(void) const;
		eMethod getMethod(void) const;
		eProcessType extractType(void) const;
		std::string getContentType(void) const;
		std::string getBody(void) const;
};

#endif
