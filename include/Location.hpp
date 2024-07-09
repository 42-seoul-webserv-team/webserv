#ifndef LOCATION_HPP
# define LOCATION_HPP

# include <map>
# include <string>
# include <vector>
# include <iostream>

# include "enum.hpp"

# define HTTP_VERSION "HTTP/1.1"

class Location
{
	private:
		std::vector<std::string> mPath;
		std::string mIndex;
		std::string mRoot;
		std::vector<eMethod> mMethod;
		std::string mRedirect;
		std::string mUpload;
		std::map<std::string, std::string> mCgi;
		bool mAutoindex;

	public:
		Location(void);
		~Location(void);
		std::vector<std::string> getPath(void);
		std::string getIndex(void);
		std::string getRoot(void);
		std::string getRedirect(void);
		std::string getUpload(void);
		std::string getCGI(std::string const & extension);
		
		bool checkMethod(eMethod reqMethod);
		bool checkIndexFile(std::vector<std::string> & url);
		bool checkAutoindex(void);
		bool checkCGI(std::string const & url);
		
		void setPath(std::vector<std::string> & path);
		void setIndex(std::string const & indexfile);
		void setRoot(std::string const & rootpath);
		void addMethod(eMethod method);
		void setRedirect(std::string const & redirection);
		void setAutoindex(bool option);
		void setUpload(std::string const & dir);
		void addCgi(std::string const & extension, std::string const & cgi);
		void clearMethod(void);
		std::string parseUrl(std::vector<std::string> & url);

		void printAll(void);
};

#endif
