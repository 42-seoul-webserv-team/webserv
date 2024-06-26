#ifndef CONFIGURE_HPP
# define CONFIGURE_HPP

// LOGGGER
# define CONFIG_ACCESS_LOG "access_log"
# define CONFIG_ERROR_LOG "error_log"

// Separator
# define NEWLINE '\n'
# define BLOCK_OPEN "{"
# define BLOCK_CLOSE "}"

// Server
# define SERVER_BLOCK "server"
# define LISTEN_PORT "listen"
# define HOST_NAME "host"
# define SERVER_NAME "server_name"
# define DEFAULT_ERROR "default_error_page"
# define LIMIT_BODY_SIZE "limit_client_body_size"

// Location
# define LOCATION_BLOCK "location"
# define ALLOW_METHODS "method"
# define ROOT_PATH "root"
# define INDEX_FILE "index"
# define AUTOINDEX "autoindex"
# define AUTOINDEX_ON "yes"
# define AUTOINDEX_OFF "no"
# define CGI_CONFIG "cgi"
# define UPLOAD_PATH "upload_path"
# define REDIRECTION "redirect"

#endif
