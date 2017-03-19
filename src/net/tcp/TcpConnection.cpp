//
// Created by beadle on 3/19/17.
//

#include <unistd.h>
#include <iostream>

#include "TcpConnection.h"
#include "base/Logging.h"
#include "net/fd/Socket.h"
#include "net/EventLoop.h"


namespace saf
{
	TcpConnection::TcpConnection(EventLoop *loop, int sockfd, int index) :
		_index(index),
		_loop(loop),
		_socket(new Socket(loop, sockfd))
	{
		_socket->setReadCallback(std::bind(&TcpConnection::handleRead, this));
		_socket->setWriteCallback(std::bind(&TcpConnection::handleWrite, this));
		_socket->setCloseCallback(std::bind(&TcpConnection::handleClose, this));
		_socket->setErrorCallback(std::bind(&TcpConnection::handleError, this));
		_socket->enableRead();
	}

	void TcpConnection::handleRead()
	{

	}

	void TcpConnection::handleWrite()
	{

	}

	void TcpConnection::handleError()
	{

	}

	void TcpConnection::handleClose()
	{
		
	}
}