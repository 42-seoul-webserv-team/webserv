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

class Connection
{
	private:
		int mSocket;
		int mServerPort;
		Server *mServer;
		Request mRequest;
		Response mResponse;
		std::string  mAbsolutePath;

		std::vector<std::vector <std::string> > mUpload; // 1.0 merge 모름

		std::string mRemainStr;
		struct timeval mTime;

		void renewTime(void);

		//juhyelee - need for run
		eStatus mStatus;
		eProcessType mProcType;
		
		std::string mCGI;

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
		eMethod getMethod(void);
		eProcessType getType(void);

		void setServer(Server *svr);
		void setStatus(eStatus status);
		void setAbsolutePath(std::string const & root, std::string const & url, std::string const & type);
		void setUpload(void);
		void setType(eProcessType type);
		void setCGI(std::string const & cgi);
		void setContentType(std::string const & type);

		void readRequest(void);
		void writeResponse(void);
		void closeSocket(void); // juhyelee - need for run because real close(fd)

		bool checkUpload(void);
		bool checkMethod(eMethod method);
		bool checkComplete(void);
		bool checkOvertime(void);
		bool checkStatus(void);

		// add 1.0 merge
		eStatus getReadStatus(void) const;
		eMethod getMethod(void) const;
		eProcessType getProcType(void) const;
		eStatus getStatus(void) const;
		int getCGIproc(void) const;
		std::string getContentType(void);
		std::string getReqBody(void) const;
		char * getAbsolutePath(void) const;
		void fillRequest(void);
		void fillRequest(std::vector<std::string> & list);
		void fillRequestCGI(void);
		void removeFile(void) const;
		void processCGI(Kqueue & kque, std::map<std::string, std::string> envp);
		void isTimeOver(void) const;
		// add 1.0 merge

		void printAll(void);
};

#endif
