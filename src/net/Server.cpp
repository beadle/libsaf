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
		_cluster(new EventLoopCluster(_loop)),
		_recvMessageCallback(nullptr),
		_connectChangeCallback(nullptr),
		_writeCompleteCallback(nullptr)
	{

	}

	Server::~Server()
	{
		assert(!_running);
	}

	void Server::bindDefaultCallbacks(Connection* conn)
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

	void Server::unbindDefaultCallbacks(Connection* conn)
	{
		conn->setCloseCallback(nullptr);
	}

	void Server::notifyConnectionEstablished(const ConnectionPtr& conn)
	{
		this->bindDefaultCallbacks(conn.get());
		conn->getLooper()->queueInLoop([this, conn]()
		{
			conn->onConnectEstablishedInLoop();

			LOG_INFO("Server(%p) create connection(%s)", this, conn->getAddress().toIpPort().c_str());
		});
	}

	void Server::notifyConnectionDestroyed(const ConnectionPtr& conn)
	{
		this->unbindDefaultCallbacks(conn.get());
		conn->getLooper()->queueInLoop([this, conn]()
		{
			conn->onConnectDestroyedInLoop();

			LOG_INFO("Server(%p) drop connection(%s)", this, conn->getAddress().toIpPort().c_str());
		});
	}

}