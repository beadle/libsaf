//
// Created by beadle on 3/15/17.
//

#ifndef LIBSAF_EPOLLPOLLER_H
#define LIBSAF_EPOLLPOLLER_H

#include <unordered_map>
#include <vector>
#include <memory>

#include "../Poller.h"


namespace saf
{

	class EPollPoller : public Poller
	{
	protected:
		EPollPoller();
		~EPollPoller();

		void poll(int timeoutMs, std::vector<IOFd*>& activeFds);

		void updateFd(IOFd *watcher) override;
		void removeFd(IOFd *watcher) override;

		friend class Poller;
		friend class EventLoop;

	protected:
		static const int kInitEventListSize = 16;

		bool hasFd(IOFd *watcher) override;
		void controlFd(int operation, IOFd *watcher);

	private:
		int _fd;

		typedef std::vector<struct epoll_event> EventList;
		typedef std::unordered_map<int, IOFd*> FdMap;

		EventList _events;
		FdMap _fds;  // for querying watcher by getFd only, doesn't own any watcher
	};

}


#endif //LIBSAF_EPOLLPOLLER_H
