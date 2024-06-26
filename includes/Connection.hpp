#ifndef CONNECTION_HPP
# define CONNECTION_HPP

# include <sys/time.h>
# include "Request.hpp"
# include "Response.hpp"

// juhyelee
# include "Server.hpp"
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

		eValidStatus mValidStatus;

		std::string mRemainStr;
		struct timeval mTime;

		void renewTime(void);

		//juhyelee - need for run
		eStatus mStatus;
		eProcessType mProcType;
	public:
		Connection(void);
		Connection(int socket, int svr_port);
		~Connection(void);

		Server *getServer(void);
		std::string getHost(void);

		void setServer(Server *svr);

		void readRequest(void);
		void validRequest(void);
		void writeResponse(void);
		void closeSock(void); // juhyelee - need for run because real close(fd)
		void access(void);
		bool checkComplete(void);
		bool checkOvertime(void);

		// juhyelee - need for run
		eStatus getReadStatus(void) const;
		eMethod getMethod(void) const;
		eProcessType getProcType(void) const;
		std::string getContentType(void) const;
		std::string getReqBody(void) const;
		char * getAbsolutePath(void) const;
		void changeStatus(eStatus const status);
		void fillRequest(void);
		void fillRequest(std::vector<std::string> & list);
		void removeFile(void) const;
		void processCGI(Kqueue & kque, std::map<std::string, std::stirng> envp[]);
};

#endif
