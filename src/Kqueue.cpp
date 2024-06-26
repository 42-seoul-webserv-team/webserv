#include "Kqueue.hpp"

Kqueue::Kqueue(void)
{
	this->mEventSize = 0;
	this->mKqIdx = 0;
	this->mKqFD = kqueue();
	if (this->mKqFD == -1)
		throw std::runtime_error("Kqueue fd failed!");
}

Kqueue::~Kqueue(void)
{
	close(this->mKqFD);
	this->mChangeList.clear();
}

int Kqueue::checkEvent(void)
{
	if (!this->mEventSize && !this->mChangeList.size())
		return 0;

	this->mKqIdx = 0;
	this->mEventSize = kevent(this->mKqFD, &(this->mChangeList.front()), this->mChangeList.size(), this->mEventList, EVENT_MAX, nullptr);
	this->mChangeList.clear();

	return this->mEventSize;
}

struct kevent *Kqueue::getEvent(void)
{
	if (this->mKqIdx >= this->mEventSize)
		return nullptr;

	return &(this->mEventList[this->mKqIdx++]);
}

void Kqueue::addEvent(int fd, void *udata)
{
	struct kevent tmp;
	if (udata == nullptr)
		EV_SET(&tmp, fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, nullptr);
	else
		EV_SET(&tmp, fd, EVFILT_READ | EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, udata);
	this->mChangeList.push_back(tmp);
}
