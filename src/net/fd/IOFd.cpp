//
// Created by beadle on 3/15/17.
//
#include <assert.h>
#include <poll.h>

#include "IOFd.h"
#include "net/EventLoop.h"


namespace saf
{
	const int IOFd::kReadEvent = POLLIN | POLLPRI;
	const int IOFd::kWriteEvent = POLLOUT;

	IOFd::IOFd(EventLoop *looper, int fd) :
		Fd(fd),
		_handling(false),
		_revents(0),
		_looper(looper),
		_status(Status::NEW)
	{

	}

	IOFd::~IOFd()
	{
		assert(!_handling);
		assert(_status == Status::ADDED);
	}

	void IOFd::enableRead()
	{
		if (!isReading())
		{
			_events |= kReadEvent;
			update();
		}
	}

	void IOFd::enableWrite()
	{
		if (!isWriting())
		{
			_events |= kWriteEvent;
			update();
		}
	}

	void IOFd::disableRead()
	{
		if (isReading())
		{
			_events &= ~kReadEvent;
			update();
		}
	}

	void IOFd::disableWrite()
	{
		if (isWriting())
		{
			_events &= ~kWriteEvent;
			update();
		}
	}

	void IOFd::disableAll()
	{
		if (!_events)
		{
			_events = 0;
			update();
		}
	}

	void IOFd::update()
	{
		_looper->updateFd(this);
	}

	void IOFd::handleEvent()
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