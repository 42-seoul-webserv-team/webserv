#ifndef LOCATION_HPP
# define LOCATION_HPP

#include <string>
#include <map>
#include <vector>

# define HTTP_VERSION "HTTP/1.1"

class Location
{
	private:
		std::vector<std::string> mPath;
		std::string mIndex;
		std::string mRoot;
		std::vector<std::string> mMethod;
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
		std::string getCgi(std::string & extension);
		
		bool checkMethod(std::string & reqMethod);
		bool checkAutoindex(void);
		
		void setPath(std::vector<std::string> & path);
		void setIndex(std::string & indexfile);
		void setRoot(std::string & rootpath);
		void addMethod(std::string & method);
		void setRedirect(std::string & redirection);
		void setAutoindex(bool option);
		void setUpload(std::string & dir);
		void addCgi(std::string & extension, std::string & cgi);
		void clearMethod(void);

};

#endif
