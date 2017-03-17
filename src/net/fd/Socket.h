//
// Created by beadle on 3/14/17.
//

#ifndef LIBSAF_SOCKET_H
#define LIBSAF_SOCKET_H

#include "../Fd.h"


namespace saf
{
	class Socket : public Fd
	{
	public:
		Socket(EventLoop* loop, int fd);

	};
}


#endif //LIBSAF_SOCKET_H
