//
// Created by beadle on 3/23/17.
//

#include "UdpConnection.h"


namespace saf
{
	UdpConnection::UdpConnection(EventLoop *loop, Socket* socket, int index):
		Connection(loop, socket, index)
	{

	}

	UdpConnection::~UdpConnection()
	{

	}
}