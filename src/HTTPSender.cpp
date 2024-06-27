#include "HTTPSender.hpp"

HTTPSender::HTTPSender()
{
}

HTTPSender::~HTTPSender()
{

}

std::string	HTTPSender::getDate(void)
{
	std::time_t	current = std::time(NULL);
	std::tm		*gmtTime = std::gmtime(&current);

	char	buffer[100];

	strftime(buffer, sizeof(buffer), "Date: %a, %d %b %Y %X GMT\r\n", gmtTime);
	return (static_cast<std::string>(buffer));
}

std::string	HTTPSender::makeMessage(Response response)
{
	struct tm	*newtime;
	std::string	message;
	
	// status line
	message += "HTTP/1.1 ";
	message += response.getCode();
	message += " ";
	message += response.getStatusMsg();
	message += "\r\n";

	// header lines
	message += this->getDate();
	message += "Server: " + response.getServerName() + "\r\n";
	message += "Connection: Closed\r\n";

	message += response.getContentLength() + "\r\n";
	if (atoi(response.getContentLength().c_str()) != 0)
		message += response.getContentType() + "\r\n";

	// Blank line
	message += "\r\n";

	message += response.getBody();
}

void	HTTPSender::sendMessage(int sockfd, const Response &response)
{
	std::string	message = this->makeMessage(response);

	send(sockfd, message.c_str(), message.size(), 0);
}

std::string	HTTPSender::makeMessage(std::string location, std::string serverName) // redirection
{
	std::string message;

	message.reserve(800);

	message += "HTTP/1.1 301 Moved Permanently\r\n";
	message += "Location: " + location + "\r\n";
	message += "Server: " + serverName;
}

void	HTTPSender::sendMessage(int sockfd, std::string location, std::string serverName) // redirection
{
	std::string	message = this->makeMessage(location, serverName);

	send(sockfd, message.c_str(), message.size(), 0);
}