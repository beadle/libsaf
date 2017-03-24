//
// Created by beadle on 3/16/17.
//

#ifndef LIBSAF_PIPE_H
#define LIBSAF_PIPE_H

#include "IOFd.h"


namespace saf
{
	class Pipe : public IOFd
	{
	public:
		Pipe(EventLoop* loop, int fd);

	};
}


#endif //LIBSAF_PIPE_H
