#ifndef CONNECTION_HPP
# define CONNECTION_HPP

# include <sys/time.h>
# include <unistd.h>
# include <vector>
# include <signal.h>
# include "Server.hpp"
# include "Request.hpp"
# include "Response.hpp"
# include "ft.hpp"
# include "enum.hpp"
# include "Kqueue.hpp"
# include "value.hpp"
# include "ConnectionException.hpp"

class Connection
{
	private:
		int mSocket;
		int mServerPort;
		int mServer;
		std::string  mAbsolutePath;
		std::string mRemainStr;
		std::vector<std::vector <std::string> > mUpload;
		std::string mCGI;
		int mCGIfd[2];
		int mCGIproc;
		
		Request mRequest;
		Response mResponse;
		eStatus mStatus;
		eProcessType mProcType;

		struct timeval mTime;
		struct timeval mCGIstart;
		
		void renewTime(void);
		char **convert(std::map<std::string, std::string> env);

	public:
		Connection(void);
		Connection(int socket, int svr_port);
		~Connection(void);

		int getSocket(void);
		int getServer(void);
		eStatus getStatus(void);
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
		std::string getQuery(void);
		int getCGISocket(void);

		void setAccept(int socket, int port);
		void setServer(int svr);
		void setServerName(std::string const & svr);
		void setStatus(eStatus status);
		void setAbsolutePath(std::string const & root, std::string const & url, std::string const & type);
		void setUpload(void);
		void setType(eProcessType type);
		void setCGI(std::string const & cgi);
		void setContentType(std::string const & type);

		void readRequest(void);
		void closeSocket(void);

		bool checkUpload(void);
		bool checkMethod(eMethod method);
		bool checkComplete(void);
		bool checkOvertime(void);
		bool checkStatus(void);

		void fillRequest(void);
		void fillRequest(std::vector<std::string> & list);
		void fillRequestCGI(void);
		void removeFile(void) const;
		void isTimeOver(void) const;
		void processCGI(Kqueue & kque, std::map<std::string, std::string> envp);
		void uploadFiles(void);
		bool checkReadDone(void);

		void printAll(void);
		
		// juhyelee - add cgi env
		void addEnv(std::map<std::string, std::string> & envp);
};

#endif
