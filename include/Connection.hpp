#ifndef CONNECTION_HPP
# define CONNECTION_HPP

# include <sys/time.h>
# include <unistd.h>

# include "Server.hpp"
# include "Request.hpp"
# include "Response.hpp"
# include "ft.hpp"
# include "enum.hpp"
# include "Kqueue.hpp"

# define CRLF "\r\n"
# define BUFFER_SIZE 4096
# define OVERTIME 15 

enum eValidStatus
{
	READY,
	METHOD,
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
		eStatus mValidStatus;

		std::vector<std::vector <std::string> > mUpload; // 1.0 merge 모름
		eRunType mType; // 1.0 merge 모름

		std::string mRemainStr;
		struct timeval mTime;

		void renewTime(void);

		//juhyelee - need for run
		eStatus mStatus;
		eProcessType mProcType;
		int mCGIfd[2];
		int mCGIproc;
		clock_t mCGIstart;

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
		// void close(void); // delete 1.0 merge
		void closeSock(void); // juhyelee - need for run because real close(fd)
//		void access(void);

		bool checkUpload(void);
		bool checkMethod(std::string const & method);
//		bool checkComplete(void);
		bool checkOvertime(void);
		bool checkStatus(void);

		// add 1.0 merge
		eStatus getReadStatus(void) const;
		eMethod getMethod(void) const;
		eProcessType getProcType(void) const;
		eStatus getStatus(void) const;
		int getCGIproc(void) const;
		std::string getContentType(void) const;
		std::string getReqBody(void) const;
		char * getAbsolutePath(void) const;
		void changeStatus(eStatus const status);
		void fillRequest(void);
		void fillRequest(std::vector<std::string> & list);
		void fillRequestCGI(void);
		void removeFile(void) const;
		void processCGI(Kqueue & kque, std::map<std::string, std::string> envp);
		bool isTimeOver(void) const;
		// add 1.0 merge

		void printAll(void);
};

#endif
