#ifndef KQUEUE_HPP
# define KQUEUE_HPP

# include <unistd.h>
# include <sys/event.h>

# include <vector>
# include <iostream>

# include "value.hpp"
# include "ManagerExcption.hpp"

class Kqueue
{
	private:
		int mKqFD;
		int mKqIdx;
		int mEventSize;
		std::vector<struct kevent> mChangeList;
		struct kevent mEventList[EVENT_MAX];

	public:
		Kqueue(void);
		~Kqueue(void);

		int checkEvent(void);
		struct kevent *getEvent(void);
		void addEvent(int fd, void *udata);
		void addCGI(int fd, void *udata);
		void changeEvent(int fd, void *udata);
};

#endif
