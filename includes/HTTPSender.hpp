#ifndef HTTPSENDER_HPP
#define HTTPSENDER_HPP

#include "../includes/Response.hpp"
#include <ctime>
#include <sys/socket.h>

class HTTPSender
{
	private:

	public:
		HTTPSender();
		~HTTPSender();

		std::string	getDate(void);
		void		sendMessage(int sockfd, const Response &response);
		std::string	makeMessage(const Response &response);
};
#endif
