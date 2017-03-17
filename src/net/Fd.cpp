//
// Created by beadle on 3/15/17.
//
#include <assert.h>
#include <poll.h>

#include "Fd.h"
#include "EventLoop.h"


namespace saf
{
	const int Fd::kReadEvent = POLLIN | POLLPRI;
	const int Fd::kWriteEvent = POLLOUT;

	Fd::Fd(EventLoop *looper, int fd) :
		_handling(false),
		_fd(fd),
		_revents(0),
		_looper(looper),
		_status(Status::NEW)
	{

	}

	Fd::~Fd()
	{
		assert(!_handling);
		assert(_status == Status::ADDED);
	}

	void Fd::enableRead()
	{
		if (!isReading())
		{
			_events |= kReadEvent;
			update();
		}
	}

	void Fd::enableWrite()
	{
		if (!isWriting())
		{
			_events |= kWriteEvent;
			update();
		}
	}

	void Fd::disableRead()
	{
		if (isReading())
		{
			_events &= ~kReadEvent;
			update();
		}
	}

	void Fd::disableWrite()
	{
		if (isWriting())
		{
			_events &= ~kWriteEvent;
			update();
		}
	}

	void Fd::disableAll()
	{
		if (!_events)
		{
			_events = 0;
			update();
		}
	}

	void Fd::update()
	{
		_looper->updateFd(this);
	}

	void Fd::handleEvent()
	{
		_handling = true;
		if ((_revents & POLLHUP) && !(_revents & POLLIN))
		{
			if (_closeCallback) _closeCallback();
		}
		if (_revents & (POLLERR | POLLNVAL))
		{
			if (_errorCallback) _errorCallback();
		}
		if (_revents & (POLLIN | POLLPRI | POLLRDHUP))
		{
			if (_readCallback) _readCallback();
		}
		if (_revents & POLLOUT)
		{
			if (_writeCallback) _writeCallback();
		}
		_handling = false;
		_revents = 0;
	}
}