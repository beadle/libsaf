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
		_connecting(false),
		_recvMessageCallback(nullptr),
		_connectChangeCallback(nullptr),
		_writeCompleteCallback(nullptr)
	{

	}

	void Client::bindDefaultCallbacks(Connection *conn)
	{
		if (_recvMessageCallback)
			conn->setRecvMessageCallback(_recvMessageCallback);

		if (_writeCompleteCallback)
			conn->setWriteCompleteCallback(_writeCompleteCallback);

		if (_connectChangeCallback)
			conn->setConnectChangeCallback(_connectChangeCallback);

		conn->setCloseCallback([this](const ConnectionPtr& connPtr)
		{
			this->onClosedInConnection(connPtr);
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