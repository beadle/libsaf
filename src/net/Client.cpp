//
// Created by beadle on 3/28/17.
//

#include "Client.h"
#include "Connection.h"
#include "Buffer.h"
#include "EventLoop.h"


namespace saf
{
	Client::Client(EventLoop *loop) :
		_loop(loop),
		_reconnectDelay(0),
		_connecting(false)
	{

	}

	void Client::bindDefaultCallbacks(Connection *ptr)
	{
		ptr->setRecvMessageCallback([this](const ConnectionPtr& conn, Buffer* buffer)
		{
			if (_recvMessageCallback)
				_recvMessageCallback(conn, buffer);
		});
		ptr->setWriteCompleteCallback([this](const ConnectionPtr& conn)
		{
			if (_writeCompleteCallback)
				_writeCompleteCallback(conn);
		});
		ptr->setConnectChangeCallback([this](const ConnectionPtr& conn)
		{
			if (_connectChangeCallback)
				_connectChangeCallback(conn);
		});
		ptr->setCloseCallback([this](const ConnectionPtr& conn)
		{
			this->onClosedInConnection(conn);
		});
	}

	void Client::unbindDefaultCallbacks(Connection *ptr)
	{
		ptr->setCloseCallback(nullptr);
	}

	void Client::notifyConnectionEstablished(const ConnectionPtr& conn)
	{
		conn->getLooper()->runInLoop([this, conn]()
		{
			this->bindDefaultCallbacks(conn.get());
			conn->onConnectEstablishedInLoop();
		});
	}

	void Client::notifyConnectionDestroyed(const ConnectionPtr& conn)
	{
		conn->getLooper()->runInLoop([this, conn]()
		{
			this->unbindDefaultCallbacks(conn.get());
			conn->onConnectDestroyedInLoop();
		});
	}
}