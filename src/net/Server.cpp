//
// Created by beadle on 3/27/17.
//
#include <assert.h>

#include "Server.h"
#include "Connection.h"
#include "EventLoop.h"
#include "EventLoopCluster.h"
#include "base/Logging.h"


namespace saf
{
	Server::Server(EventLoop *loop) :
		_running(false),
		_loop(loop),
		_cluster(new EventLoopCluster())
	{

	}

	Server::~Server()
	{
		assert(!_running);
	}

	void Server::bindDefaultCallbacks(Connection* conn)
	{
		conn->setRecvMessageCallback([this](const ConnectionPtr& connPtr, Buffer* buffer)
		{
			if (_recvMessageCallback)
				_recvMessageCallback(connPtr, buffer);
		});
		conn->setWriteCompleteCallback([this](const ConnectionPtr& connPtr)
		{
			if (_writeCompleteCallback)
				_writeCompleteCallback(connPtr);
		});
		conn->setConnectChangeCallback([this](const ConnectionPtr& connPtr)
		{
			if (_connectChangeCallback)
				_connectChangeCallback(connPtr);
		});
		conn->setCloseCallback([this](const ConnectionPtr& connPtr)
		{
			this->onClosedInConnection(connPtr);
		});
	}

	void Server::unbindDefaultCallbacks(Connection* conn)
	{
		conn->setCloseCallback(nullptr);
	}

	void Server::notifyConnectionEstablished(const ConnectionPtr& conn)
	{
		conn->getLooper()->queueInLoop([this, conn]()
		{
			this->bindDefaultCallbacks(conn.get());
			conn->onConnectEstablishedInLoop();

			LOG_INFO("Server(%p) create connection(%s)", this, conn->getAddress().toIpPort().c_str());
		});
	}

	void Server::notifyConnectionDestroyed(const ConnectionPtr& conn)
	{
		conn->getLooper()->queueInLoop([this, conn]()
		{
			this->unbindDefaultCallbacks(conn.get());
			conn->onConnectDestroyedInLoop();

			LOG_INFO("Server(%p) drop connection(%s)", this, conn->getAddress().toIpPort().c_str());
		});
	}

}