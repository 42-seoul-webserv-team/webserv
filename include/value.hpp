#ifndef VALUE_HPP
# define VALUE_HPP

// Websev
# define DEFAULT_CONFIG "conf/default.conf"
# define PORT_MIN 1024
# define PORT_MAX 49151
# define LISTEN_MAX 10

// Kqueue
# define EVENT_MAX 30

// Server
# define DEFAULT_SERVERNAME "WebServ"
# define DEFAULT_BODY_SIZE "4K"

// Connection
# define CRLF "\r\n"
# define BUFFER_SIZE 4096
# define CGI_OVERTIME 5
# define REQ_OVERTIME 10

// Logger
# define DEFAULT_ACCESS_LOG "access.log"
# define DEFAULT_ERROR_LOG "error.log"

// Request
# define HEADER_LENGTH_MAX 1000000
# define URL_LENGTH_MAX 1000000


#endif
