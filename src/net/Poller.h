//
// Created by beadle on 3/15/17.
//

#ifndef LIBSAF_POLLER_H
#define LIBSAF_POLLER_H

#include <functional>
#include <vector>
#include <base/Noncopyable.h>

#include "net/fd/IOFd.h"

namespace saf
{
	class Poller : public Noncopyable
	{
	public:
		static Poller* createPoller();

	public:
		virtual ~Poller() {};

		virtual void poll(int timeoutMs, std::vector<IOFd*>& activeFds) = 0;

		virtual bool hasFd(IOFd *fd) = 0;
		virtual void updateFd(IOFd *fd) = 0;
		virtual void removeFd(IOFd *fd) = 0;
	};

}


#endif //LIBSAF_POLLER_H
