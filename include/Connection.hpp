#ifndef CONNECTION_HPP
# define CONNECTION_HPP

# include <fcntl.h>
# include <unistd.h>
# include <stdlib.h>
# include <signal.h>
# include <dirent.h>
# include <sys/time.h>
# include <sys/socket.h>

# include <deque>
# include <vector>
# include <string>
# include <sstream>
# include <iostream>
# include <algorithm>

# include "ft.hpp"
# include "enum.hpp"
# include "value.hpp"
# include "Kqueue.hpp"
# include "Server.hpp"
# include "Request.hpp"
# include "Response.hpp"
# include "ConnectionException.hpp"

class Connection
{
	private:
		int mSocket;
		int mServerPort;
		int mServer;
		std::string  mAbsolutePath;
		std::deque<char> mRemain;
		std::string mCGI;
		std::string mCGIfile;
		int mCGIfd;
		int mCGIproc;
		
		std::vector<std::vector <char> > mUpload;
		std::vector<std::vector <std::string> > mUploadInfo;
		std::string mUploadStart;
		std::string mUploadEnd;
		eStatus mUploadStatus;

		Request mRequest;
		Response mResponse;
		eStatus mStatus;
		eProcessType mProcType;

		struct timeval mTime;
		struct timeval mCGIstart;
		
		void renewTime(void);
		char **convert(std::map<std::string, std::string> env);
		void setBody(void);
		void setUpload(void);
		void setUploadData(std::string & st, std::vector<char> & vt);

	public:
		Connection(void);
		Connection(int socket, int svr_port);
		~Connection(void);

		int getSocket(void);
		int getServer(void);
		int getPort(void);
		std::string getHost(void);
		std::string getUrl(void);
		eMethod getMethod(void);
		eProcessType getType(void);
		eStatus getReadStatus(void) const;
		eMethod getMethod(void) const;
		eProcessType getProcType(void) const;
		eStatus getStatus(void) const;
		int getCGIproc(void) const;
		std::string getContentType(void);
		std::string getReqBody(void) const;
		Response getResponse(void) const;
		char * getAbsolutePath(void) const;
		int getBodySize(void);
		int getCGISocket(void);
		std::string getMessage(void);

		void setAccept(int socket, int port);
		void setServer(int svr);
		void setServerName(std::string const & svr);
		void setStatus(eStatus status);
		void setAbsolutePath(std::string const & root, std::string const & url, std::string const & type);
		void setType(eProcessType type);
		void setCGI(std::string const & cgi);
		void setContentType(std::string const & type);
		void setMessage(std::string const & message);

		void readRequest(void);
		void closeSocket(void);

		bool checkUpload(void);
		bool checkMethod(eMethod method);
		bool checkComplete(void);
		bool checkOvertime(void);
		bool checkStatus(void);
		bool checkUploadSize(int size);

		void fillRequest(void);
		void fillRequest(std::vector<std::string> & list);
		void fillRequestCGI(void);
		void removeFile(void) const;
		void isTimeOver(void) const;
		void processCGI(Kqueue & kque, std::map<std::string, std::string> envp);
		void uploadFiles(void);
		bool checkReadDone(void);

		void printAll(void);

		void addEnv(std::map<std::string, std::string> & envp);
};

#endif
