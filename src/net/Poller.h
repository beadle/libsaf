//
// Created by beadle on 3/15/17.
//

#ifndef LIBSAF_POLLER_H
#define LIBSAF_POLLER_H

#include <functional>
#include <vector>

#include "net/fd/IOFd.h"

namespace saf
{
	class Poller
	{
	public:
		static Poller* createPoller();

	public:
		virtual ~Poller() {};

		virtual std::vector<IOFd*> poll(int timeoutMs) = 0;

		virtual bool hasWatcher(IOFd* watcher) = 0;
		virtual void updateWatcher(IOFd* watcher) = 0;
		virtual void removeWatcher(IOFd* watcher) = 0;
	};

}


#endif //LIBSAF_POLLER_H
