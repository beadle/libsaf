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

	IOFd::IOFd(int fd) :
		Fd(fd),
		_looper(nullptr),
		_handling(false),
		_revents(0),
		_status(Status::NEW)
	{

	}

	IOFd::~IOFd()
	{
		assert(!_handling);
		assert(_status != Status::ADDED);
		assert(isNoneEvent());
	}

	void IOFd::enableReadInLoop()
	{
		if (!isReading())
		{
			_events |= kReadEvent;
			update();
		}
	}

	void IOFd::enableWriteInLoop()
	{
		if (!isWriting())
		{
			_events |= kWriteEvent;
			update();
		}
	}

	void IOFd::disableReadInLoop()
	{
		if (isReading())
		{
			_events &= ~kReadEvent;
			update();
		}
	}

	void IOFd::disableWriteInLoop()
	{
		if (isWriting())
		{
			_events &= ~kWriteEvent;
			update();
		}
	}

	void IOFd::disableAllInLoop()
	{
		if (_events)
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
			goto end;
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

		end:
		_handling = false;
		_revents = 0;
	}

	void IOFd::attachInLoop(EventLoop *looper)
	{
		assert(looper);
		looper->assertInLoopThread();

		if (looper == _looper)
		{
			return;
		}

		if (_looper)
		{
			detachInLoop();
		}

		_looper = looper;
		if (_events)
		{
			update();
		}
	}

	bool IOFd::detachInLoop()
	{
		if (_looper)
		{
			_looper->assertInLoopThread();

			disableAllInLoop();
			_looper->removeFd(this);
			_looper = nullptr;
			return true;
		}
		else
		{
			return false;
		}
	}

}