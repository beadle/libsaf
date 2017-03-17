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

		std::vector<Fd*> poll(int timeoutMs);

	public: // exposed to Fd only
		bool hasWatcher(Fd* watcher) override;
		void updateWatcher(Fd* watcher) override;
		void removeWatcher(Fd* watcher) override;

	protected:
		static const int kInitEventListSize = 16;

		void controlFd(int operation, Fd *watcher);

	private:
		int _fd;

		typedef std::vector<struct epoll_event> EventList;
		typedef std::unordered_map<int, Fd*> WatcherMap;

		EventList _events;
		WatcherMap _watchers;  // for querying watcher by getFd only, doesn't own any watcher
	};

}


#endif //LIBSAF_EPOLLPOLLER_H
