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
	public:
		EPollPoller();
		~EPollPoller();

		std::vector<IOFd*> poll(int timeoutMs);

	public: // exposed to IOFd only
		bool hasWatcher(IOFd* watcher) override;
		void updateWatcher(IOFd* watcher) override;
		void removeWatcher(IOFd* watcher) override;

	protected:
		static const int kInitEventListSize = 16;

		void controlFd(int operation, IOFd *watcher);

	private:
		int _fd;

		typedef std::vector<struct epoll_event> EventList;
		typedef std::unordered_map<int, IOFd*> WatcherMap;

		EventList _events;
		WatcherMap _watchers;  // for querying watcher by getFd only, doesn't own any watcher
	};

}


#endif //LIBSAF_EPOLLPOLLER_H
