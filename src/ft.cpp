#include "ft.hpp"

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

int	ft::toInt(std::string const & str, int unit)
{
	std::string org = str;
	long long	ret = 0;
	std::string form = "0123456789abcdef";
	int			minus = 1;
	size_t		pos;

	if (unit < 2 || 16 < unit)
		throw static_cast<int>(ret);
	form.resize(unit);
	ft::trim(org);

	while (!org.empty() 
			&& (*org.begin() == '-' || *org.begin() == '+'))
	{
		if (*org.begin() == '-')
			minus *= -1;
		org.erase(0, 1);
	}

	while (!org.empty())
	{
		pos = form.find(*org.begin());
		if (pos == std::string::npos)
			throw static_cast<int>(ret);
		org.erase(0, 1);
		ret *= unit;
		ret += pos * minus;
	}
	return static_cast<int>(ret);
}

std::string ft::toString(long long nbr, int unit)
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
	if (nbr == 0)
		return "0";
	while (nbr)
	{
		ret.insert(ret.begin(), form[nbr % unit * minus]);
		nbr /= unit;
	}
	return sign + ret;
}
