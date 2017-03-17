//
// Created by beadle on 3/15/17.
//

#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <sys/epoll.h>

#include "EPollPoller.h"

namespace saf
{
	EPollPoller::EPollPoller() :
		_fd(::epoll_create1(EPOLL_CLOEXEC)),
		_events(kInitEventListSize)
	{

	}

	EPollPoller::~EPollPoller()
	{
		::close(_fd);
	}

	bool EPollPoller::hasWatcher(Fd *watcher)
	{
		auto it = _watchers.find(watcher->getFd());
		return it != _watchers.end() && it->second == watcher;
	}

	void EPollPoller::updateWatcher(Fd *watcher)
	{
		auto status = watcher->getStatus();
		auto fd = watcher->getFd();

		if (status == Fd::Status::NEW)
		{
			assert(_watchers.find(fd) == _watchers.end());

			watcher->setStatus(Fd::Status::ADDED);
			_watchers[fd] = watcher;
			controlFd(EPOLL_CTL_ADD, watcher);
		}
		else if(status == Fd::Status::DELETED)
		{
			assert(hasWatcher(watcher));

			watcher->setStatus(Fd::Status::ADDED);
			controlFd(EPOLL_CTL_ADD, watcher);
		}
		else
		{
			assert(hasWatcher(watcher));

			if (watcher->isNoneEvent())
			{
				controlFd(EPOLL_CTL_DEL, watcher);
				watcher->setStatus(Fd::Status::DELETED);
			}
			else
			{
				controlFd(EPOLL_CTL_MOD, watcher);
			}
		}
	}

	void EPollPoller::removeWatcher(Fd *watcher)
	{
		assert(hasWatcher(watcher));
		assert(watcher->isNoneEvent());
		assert(watcher->getStatus() == Fd::Status::ADDED
			   || watcher->getStatus() == Fd::Status::DELETED);

		_watchers.erase(watcher->getFd());

		if (watcher->getStatus() == Fd::Status::ADDED)
			controlFd(EPOLL_CTL_DEL, watcher);

		watcher->setStatus(Fd::Status::NEW);
	}

	std::vector<Fd*> EPollPoller::poll(int timeoutMs)
	{
		std::vector<Fd*> watchers;

		int numEvents = ::epoll_wait(_fd, &*_events.begin(), static_cast<int>(_events.size()), timeoutMs);
		if (numEvents > 0)
		{
			for (int i = 0; i < numEvents; ++i)
			{
				Fd* watcher = static_cast<Fd*>(_events[i].data.ptr);
				watcher->setREvent(_events[i].events);
				watchers.push_back(watcher);
			}

			if (static_cast<size_t>(numEvents) == _events.size())
			{
				_events.reserve(_events.size() * 2);
			}
		}

		return watchers;
	}

	void EPollPoller::controlFd(int operation, Fd *watcher)
	{
		struct epoll_event event;
		bzero(&event, sizeof event);

		event.events = watcher->getEvents();
		event.data.ptr = watcher;

		::epoll_ctl(_fd, operation, watcher->getFd(), &event);
	}

}