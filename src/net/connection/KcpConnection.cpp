//
// Created by beadle on 3/23/17.
//

#include "KcpConnection.h"

namespace saf
{
	KcpConnection::KcpConnection(EventLoop *loop, Socket* socket, int index) :
		Connection(loop, socket, index)
	{

	}
}