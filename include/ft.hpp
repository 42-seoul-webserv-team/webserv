#ifndef FT_HPP
# define FT_HPP

# include <string>
# include <vector>
# include <iostream>

# define WHITE_SPACE " \t\n\r\f\v"

namespace ft
{
	std::string	trim(std::string & str);
	std::vector<std::string> split(std::string const & str);
	std::vector<std::string> split(std::string const & str, std::string const & delim);
	int toInt(std::string const & str, int unit);
	std::string toString(long long nbr, int unit);
}

#endif
