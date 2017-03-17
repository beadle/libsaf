//
// Created by beadle on 3/15/17.
//

#ifndef LIBSAF_POLLER_H
#define LIBSAF_POLLER_H

#include <functional>
#include <vector>

#include "Fd.h"

namespace saf
{
	class Poller
	{
	public:
		static Poller* createPoller();

	public:
		virtual ~Poller() {};

		virtual std::vector<Fd*> poll(int timeoutMs) = 0;

		virtual bool hasWatcher(Fd* watcher) = 0;
		virtual void updateWatcher(Fd* watcher) = 0;
		virtual void removeWatcher(Fd* watcher) = 0;
	};

}


#endif //LIBSAF_POLLER_H
