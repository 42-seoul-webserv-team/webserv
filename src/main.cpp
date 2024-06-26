#include "main.hpp"

void my::signalHandler(int signal)
{
	(void)signal;
	gSignal = false;
}

int main(int argc, char *argv[], char *envp[])
{
	try {
		gSignal = true;
		signal(SIGQUIT, my::signalHandler);

		WebServ manager;

		if (argc - 1 == 0)
			manager.configure(DEFAULT_CONFIG);
		else
			manager.configure(argv[1]);

		while (gSignal)
			manager.activate(envp);

	} catch (std::exception &e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}
	return 0;
}
