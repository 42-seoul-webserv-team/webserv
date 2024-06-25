#include "../includes/HTTPSender.hpp"

HTTPSender::HTTPSender()
{
}

HTTPSender::~HTTPSender()
{

}

std::string	getDate(void)
{
	std::time_t	current = std::time(NULL);
	std::tm		*gmtTime = std::gmtime(&current);

	char	buffer[100];

	strftime(buffer, sizeof(buffer), "Date: %a, %d %b %Y %X GMT\r\n", gmtTime);
	return (static_cast<std::string>(buffer));
}

std::string	HTTPSender::makeMessage(const Response &response)
{
	struct tm	*newtime;
	time_t		ltime;
	std::string	message = "HTTP/1.1 ";

	ltime = time(&ltime);
	newtime = localtime(&ltime);
	// reserve 이용해서 최적화?

	// status line
	message += response.getCode();
	message += " ";
	message += response.getStatusMsg();
	message += "\r\n";

	// header lines
	message += this->getDate();
	message = "Server: Webserv\r\n";
	message = 
	message = "Connection: close"; // 무지성
	// content-length 대기
	// content-type 대기

	// Blank line
	message += "\r\n";


	// Entity body (optional)
}

void	HTTPSender::sendMessage(int sockfd, const Response &response)
{
	std::string	message = this->makeMessage(response);

	send(sockfd, message.c_str(), message.size(), 0);
}