//
// Created by beadle on 3/19/17.
//

#include <unistd.h>
#include <iostream>
#include <string.h>
#include <map>

#include "TcpConnection.h"
#include "base/Logging.h"
#include "net/fd/Socket.h"
#include "net/EventLoop.h"


namespace saf
{
	TcpConnection::TcpConnection(EventLoop *loop, Socket* socket, int index):
		Connection(loop, socket, index)
	{
		socket->setKeepAlive(true);
	}

	TcpConnection::~TcpConnection()
	{

	}
}