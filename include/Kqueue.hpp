#ifndef KQUEUE_HPP
# define KQUEUE_HPP

# include <sys/event.h>
# include <unistd.h>
# include <vector>
# include "ManagerExcption.hpp"
# include "value.hpp"

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
};

#endif
