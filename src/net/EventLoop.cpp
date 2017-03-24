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
#include "base/Logging.h"


namespace
{
	int createEventfd()
	{
		int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
		if (evtfd < 0)
		{
			LOG_ERROR("createEventfd")
			abort();
		}
		return evtfd;
	}
}


namespace saf
{
	__thread EventLoop* t_looperInThread = nullptr;
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
		LOG_DEBUG("EventLoop(%p) created in this thread(%d)", this, _threadId);
		if (t_looperInThread)
		{
			LOG_FATAL("Another EventLoop(%p) exists in this thread(%d)", this, _threadId);
		}
		else
		{
			t_looperInThread = this;
		}

		_wakeupFd->enableRead();
		_wakeupFd->setReadCallback(std::bind(&EventLoop::handleWakeupRead, this));
	}

	EventLoop::~EventLoop()
	{
		assertInLoopThread();
		LOG_DEBUG("EventLoop(%p) of thread(%d) destructs", this, _threadId);

		_wakeupFd->disableAll();
		removeFd(_wakeupFd.get());
		::close(_wakeupFd->getFd());

		runFunctors();
		runTimers();
	}

	void EventLoop::start()
	{
		assert(!_looping);
		assert(!_quit);

		_quit = false;
		_looping = true;

		LOG_DEBUG("EventLoop(%p) of thread(%d) connect looping", this, _threadId);

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
		LOG_DEBUG("EventLoop(%p) of thread(%d) disconnect looping", this, _threadId);

	}

	void EventLoop::stop()
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

	int EventLoop::addTimer(float delay, const Functor& callback, bool repeated)
	{
		auto timer = _timerQueue->createTimer(delay, callback, repeated);
		runInLoop([this, timer](){
			_timerQueue->addTimer(timer);
		});
		return timer->getFd();
	}

	void EventLoop::cancelTimer(int fd)
	{
		runInLoop([this, fd](){
			_timerQueue->cancelTimer(fd);
		});
	}

	void EventLoop::queueInLoop(Functor &&functor)
	{
		std::lock_guard<std::mutex> guard(_mutex);
		_functors.push_back(functor);
	}

	void EventLoop::updateFd(IOFd *fd)
	{
		assert(fd->getLooper() == this);
		assertInLoopThread();
		_poller->updateFd(fd);
	}

	void EventLoop::removeFd(IOFd *fd)
	{
		assert(fd->getLooper() == this);
		assertInLoopThread();
		_poller->removeFd(fd);
	}

	void EventLoop::handleWakeupRead()
	{
		uint64_t one = 1;
		ssize_t n = ::read(_wakeupFd->getFd(), &one, sizeof one);
		if (n != 8)
		{
			LOG_ERROR("EventLoop::wakeup() reads %d bytes instead of 8", n);
		}
	}

	void EventLoop::wakeup()
	{
		uint64_t one = 1;
		ssize_t n = ::write(_wakeupFd->getFd(), &one, sizeof one);
		if (n != 8)
		{
			LOG_ERROR("EventLoop::wakeup() writes %d bytes instead of 8", n);
		}
	}

	void EventLoop::abortNotInLoopThread()
	{
		LOG_FATAL("EventLoop::abortNotInLoopThread - EventLoop(%p) of thread(%d) tid of CurrentThread(%d)",
				  this, _threadId, CurrentThread::tid());
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