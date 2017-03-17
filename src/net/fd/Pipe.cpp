//
// Created by beadle on 3/16/17.
//

#include "Pipe.h"
#include "../EventLoop.h"


namespace saf
{
	Pipe::Pipe(EventLoop *loop, int fd)
		: Fd(loop, fd)
	{

	}
}