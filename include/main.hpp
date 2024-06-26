#ifndef MAIN_HPP
# define MAIN_HPP

# include <signal.h>
# include <iostream>
# include "WebServ.hpp"

bool gSignal;

namespace my
{
	void signalHandler(int signal);
};

#endif
