//
// Created by beadle on 3/15/17.
//

#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <sys/epoll.h>
#include <iostream>

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

	bool EPollPoller::hasFd(IOFd *watcher)
	{
		auto it = _fds.find(watcher->getFd());
		return it != _fds.end() && it->second == watcher;
	}

	void EPollPoller::updateFd(IOFd *watcher)
	{
		auto status = watcher->getStatus();
		auto fd = watcher->getFd();

		if (status == IOFd::Status::NEW)
		{
			assert(_fds.find(fd) == _fds.end());

			watcher->setStatus(IOFd::Status::ADDED);
			_fds[fd] = watcher;
			controlFd(EPOLL_CTL_ADD, watcher);
		}
		else if(status == IOFd::Status::DELETED)
		{
			assert(hasFd(watcher));

			watcher->setStatus(IOFd::Status::ADDED);
			controlFd(EPOLL_CTL_ADD, watcher);
		}
		else
		{
			assert(hasFd(watcher));

			if (watcher->isNoneEvent())
			{
				controlFd(EPOLL_CTL_DEL, watcher);
				watcher->setStatus(IOFd::Status::DELETED);
			}
			else
			{
				controlFd(EPOLL_CTL_MOD, watcher);
			}
		}
	}

	void EPollPoller::removeFd(IOFd *watcher)
	{
		if (!hasFd(watcher))
			return;

		assert(watcher->isNoneEvent());
		assert(watcher->getStatus() == IOFd::Status::ADDED
			   || watcher->getStatus() == IOFd::Status::DELETED);

		_fds.erase(watcher->getFd());

		if (watcher->getStatus() == IOFd::Status::ADDED)
			controlFd(EPOLL_CTL_DEL, watcher);

		watcher->setStatus(IOFd::Status::NEW);
	}

	std::vector<IOFd*> EPollPoller::poll(int timeoutMs)
	{
		std::vector<IOFd*> watchers;

		int numEvents = ::epoll_wait(_fd, &*_events.begin(), static_cast<int>(_events.size()), timeoutMs);
		if (numEvents > 0)
		{
			for (int i = 0; i < numEvents; ++i)
			{
				IOFd* watcher = static_cast<IOFd*>(_events[i].data.ptr);
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

	void EPollPoller::controlFd(int operation, IOFd *watcher)
	{
		struct epoll_event event;
		bzero(&event, sizeof event);

		event.events = watcher->getEvents();
		event.data.ptr = watcher;

		::epoll_ctl(_fd, operation, watcher->getFd(), &event);
	}

}