//
// Created by beadle on 3/14/17.
//

#include "Socket.h"
#include "../EventLoop.h"

namespace saf
{

	Socket::Socket(EventLoop *loop, int fd):
		Fd(loop, fd)
	{

	}

}