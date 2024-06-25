#ifndef CONNECTION_HPP
# define CONNECTION_HPP

# include <sys/time.h>
# include <unistd.h>
# include "Server.hpp"
# include "Request.hpp"
# include "Response.hpp"
# include "ft.hpp"

# define BUFFER_SIZE 4096
# define OVERTIME 15 

enum eRunType
{
	NONE,
	FILES,
	AUTOINDEX,
	CGI,
	UPLOAD,
};

class Connection
{
	private:
		int mSocket;
		int mServerPort;
		Server *mServer;
		Request mRequest;
		Response mResponse;
		
		std::string  mAbsolutePath;
		std::vector<std::vector <std::string> > mUpload;

		eStatus mValidStatus;
		eRunType mType;
	

		std::string mRemainStr;
		struct timeval mTime;

		void renewTime(void);

	public:
		Connection(void);
		Connection(int socket, int svr_port);
		~Connection(void);

		int getSocket(void);
		Server *getServer(void);
		eStatus getStatus(void);
		int getPort(void);
		std::string getHost(void);
		std::string getUrl(void);
		std::string getMethod(void);
		std::string getAbsoultePath(void);
		eRunType getType(void);

		void setServer(Server *svr);
		void setStatus(eStatus status);
		void setAbsoultePath(std::string const & root, std::string const & url, std::string const & type);
		void setUpload(void);
		void setType(eRunType type);

		void readRequest(void);
		void writeResponse(void);
		void close(void);
//		void access(void);

		bool checkUpload(void);
		bool checkMethod(std::string const & method);
//		bool checkComplete(void);
		bool checkOvertime(void);
		bool checkStatus(void);

		void printAll(void);
};

#endif
