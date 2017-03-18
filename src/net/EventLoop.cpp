//
// Created by beadle on 3/16/17.
//

#include <unistd.h>
#include <sys/eventfd.h>
#include <iostream>

#include "net/fd/IOFd.h"
#include "Poller.h"
#include "EventLoop.h"
#include "TimerQueue.h"


namespace
{
	int createEventfd()
	{
		int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
		if (evtfd < 0)
		{
			abort();
		}
		return evtfd;
	}

}


namespace saf
{

	const int kPollTimeMs = 33;

	EventLoop::EventLoop() :
		_threadId(CurrentThread::tid()),
		_quit(false),
		_looping(false),
		_handling(false),
		_currentFd(nullptr),
		_wakeupFd(new IOFd(this, createEventfd())),
		_poller(Poller::createPoller()),
		_timerQueue(new TimerQueue(this))
	{
		_wakeupFd->enableRead();
		_wakeupFd->setReadCallback(std::bind(&EventLoop::handleWakeupRead, this));
	}

	EventLoop::~EventLoop()
	{
		_wakeupFd->disableAll();
		removeFd(_wakeupFd.get());
		::close(_wakeupFd->getFd());
	}

	void EventLoop::run()
	{
		assert(!_looping);
		_quit = false;
		_looping = true;

		while (!_quit)
		{
			auto fds = _poller->poll(kPollTimeMs);
			_handling = true;
			auto activeFds = _poller->poll(kPollTimeMs);
			for (auto fd : activeFds)
			{
				_currentFd = fd;
				_currentFd->handleEvent();
			}
			_currentFd = nullptr;
			_handling = false;

			runFunctors();
			runTimers();
		}

		_looping = false;
	}

	void EventLoop::quit()
	{
		_quit = true;
	}

	void EventLoop::runInLoop(Functor &&functor)
	{
		if (isInLoopThread())
		{
			functor();
		}
		else
		{
			queueInLoop(std::move(functor));
		}
	}

	int EventLoop::addTimer(float delay, Functor &&callback, bool repeated)
	{
		return _timerQueue->addTimer(delay, std::move(callback), repeated);
	}

	void EventLoop::cancelTimer(int fd)
	{
		_timerQueue->cancelTimer(fd);
	}

	void EventLoop::queueInLoop(Functor &&functor)
	{
		std::lock_guard<std::mutex> guard(_mutex);
		_functors.push_back(functor);
	}

	bool EventLoop::hasFd(IOFd *fd)
	{
		assert(fd->getLooper() == this);
		assertInLoopThread();
		return _poller->hasWatcher(fd);
	}

	void EventLoop::updateFd(IOFd *fd)
	{
		assert(fd->getLooper() == this);
		assertInLoopThread();
		_poller->updateWatcher(fd);
	}

	void EventLoop::removeFd(IOFd *fd)
	{
		assert(fd->getLooper() == this);
		assertInLoopThread();
		_poller->removeWatcher(fd);
	}

	void EventLoop::handleWakeupRead()
	{
		uint64_t one = 1;
		ssize_t n = ::read(_wakeupFd->getFd(), &one, sizeof one);
		assert(n == 8);

		std::cout << "[EventLoop::handleWakeupRead] one: " << one << std::endl;
	}

	void EventLoop::wakeup()
	{
		uint64_t one = 1;
		ssize_t n = ::write(_wakeupFd->getFd(), &one, sizeof one);
		assert(n == 8);
	}

	void EventLoop::runFunctors()
	{
		std::vector<Functor> functors;
		{
			std::lock_guard<std::mutex> guard(_mutex);
			functors.swap(_functors);
		}

		for (auto functor : functors)
		{
			functor();
		}
	}

	void EventLoop::runTimers()
	{
		while (_timerQueue->checkTimers())
		{
			_timerQueue->popTimer();
		}
	}

}