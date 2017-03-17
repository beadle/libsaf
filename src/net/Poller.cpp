//
// Created by beadle on 3/15/17.
//

#include "Poller.h"

#include "poller/EPollPoller.h"


namespace saf
{
	Poller* Poller::createPoller()
	{
		return new EPollPoller();
	}
}