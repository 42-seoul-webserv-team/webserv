#ifndef FT_HPP
# define FT_HPP

# include <string>
# include <vector>
# include <exception>

# define WHITE_SPACE " \t\n\r\f\v"

namespace ft
{
	std::string	trim(std::string & str);
	std::vector<std::string> split(std::string str);
	std::vector<std::string> split(std::string str, std::string const & delim);
	int toInt(std::string str, int unit);
	std::string toString(ssize_t nbr, int unit);
}

#endif
