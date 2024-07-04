#include "Kqueue.hpp"

Kqueue::Kqueue(void)
{
	this->mEventSize = 0;
	this->mKqIdx = 0;
	this->mKqFD = kqueue();
	if (this->mKqFD == -1)
		throw ManagerException("Kqueue fd failed!");
}

Kqueue::~Kqueue(void)
{
	close(this->mKqFD);
	this->mChangeList.clear();
}

#include <iostream>

int Kqueue::checkEvent(void)
{
	if (!this->mEventSize && !this->mChangeList.size())
		return 0;

	this->mKqIdx = 0;
	this->mEventSize = kevent(this->mKqFD, &(this->mChangeList.front()), this->mChangeList.size(), this->mEventList, EVENT_MAX, NULL);
	this->mChangeList.clear();

	return this->mEventSize;
}

struct kevent *Kqueue::getEvent(void)
{
	if (this->mKqIdx >= this->mEventSize)
		return NULL;

	std::cout << "Socket: " << this->mEventList[this->mKqIdx].ident << std::endl;
	std::cout << "Flag: ";
	switch (this->mEventList[this->mKqIdx].filter)
	{
		case EVFILT_READ:
			std::cout << "READ" << std::endl;
			break ;
		case EVFILT_WRITE:
			std::cout << "WRITE" << std::endl;
			break ;
		default:
			std::cout << "??" << std::endl;
			break;
	}
	return &(this->mEventList[this->mKqIdx++]);
}

void Kqueue::addEvent(int fd, void *udata)
{
	if (udata == NULL)
	{
		struct kevent tmp;
		EV_SET(&tmp, fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
		this->mChangeList.push_back(tmp);
	}
	else
	{
		{
			struct kevent tmp;
			EV_SET(&tmp, fd, EVFILT_READ, EV_ADD | EV_ENABLE | EV_EOF, 0, 0, udata);
			this->mChangeList.push_back(tmp);
		}
		/*
		{
			struct kevent tmp;
			EV_SET(&tmp, fd, EVFILT_WRITE, EV_ADD | EV_DISABLE | EV_EOF, 0, 0, udata);
			this->mChangeList.push_back(tmp);
		}*/
	}
}

void Kqueue::addCGI(int fd, void *udata)
{
	if (udata == NULL)
		return ;

	struct kevent tmp;

	EV_SET(&tmp, fd, EVFILT_WRITE, EV_ADD | EV_ENABLE | EV_EOF, 0, 0, udata);
	this->mChangeList.push_back(tmp);
}

void Kqueue::changeEvent(int fd, void *udata)
{
	if (udata == NULL)
		return ;

	{
		struct kevent tmp;
		EV_SET(&tmp, fd, EVFILT_WRITE, EV_ADD | EV_ENABLE , 0, 0, udata);
		this->mChangeList.push_back(tmp);
	}
	{
		struct kevent tmp;
		EV_SET(&tmp, fd, EVFILT_READ, EV_DISABLE , 0, 0, udata);
		this->mChangeList.push_back(tmp);
	}
}
