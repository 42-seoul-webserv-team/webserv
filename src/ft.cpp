#include "ft.hpp"
#include <iostream>

std::string ft::trim(std::string & str)
{
	str.erase(0, str.find_first_not_of(WHITE_SPACE));
	str.erase(str.find_last_not_of(WHITE_SPACE) + 1);
	return str;
}

std::vector<std::string> ft::split(std::string const & str)
{
	return ft::split(str, WHITE_SPACE);
}

std::vector<std::string> ft::split(std::string const & str, std::string const & delim)
{
	std::string org = str;
	std::vector<std::string> ret;
	org.erase(0, org.find_first_not_of(delim));
	while (!org.empty())
	{
		ret.push_back(org.substr(0, org.find_first_of(delim)));
		org.erase(0, ret.back().size());
		org.erase(0, org.find_first_not_of(delim));
	}
	return ret;
}

int	ft::toInt(std::string str, int unit)
{
	long long	ret = 0;
	std::string form = "0123456789abcdef";
	int			minus = 1;
	size_t		pos;

	if (unit < 2 || 16 < unit)
		throw static_cast<int>(ret);
	form.resize(unit);
	ft::trim(str);

	while (!str.empty() 
			&& (str.front() == '-' || str.front() == '+'))
	{
		if (str.front() == '-')
			minus *= -1;
		str.erase(0, 1);
	}

	while (!str.empty())
	{
		pos = form.find(str.front());
		if (pos == std::string::npos)
			throw static_cast<int>(ret);
		str.erase(0, 1);
		ret *= unit;
		ret += pos * minus;
		if (ret < INT_MIN || ret > INT_MAX)
			throw static_cast<int>(ret / unit);
	}
	return static_cast<int>(ret);
}

std::string ft::toString(ssize_t nbr, int unit)
{
	std::string ret = "";
	std::string sign = "";
	std::string form = "0123456789abcef";
	int minus = 1;

	if (unit < 2 || 16 < unit)
		return ret;
	form.resize(unit);

	if (nbr < 0)
	{
		sign = "-";
		minus = -1;
	}
	while (nbr)
	{
		ret.insert(ret.begin(), form[nbr % unit * minus]);
		nbr /= unit;
	}
	return sign + ret;
}
