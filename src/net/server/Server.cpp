//
// Created by beadle on 3/19/17.
//

#include "Server.h"
#include "net/Connection.h"

#include "base/Logging.h"
#include "net/EventLoop.h"
#include "net/EventLoopCluster.h"
#include "Acceptor.h"
#include "net/fd/Socket.h"

#include "net/connection/TcpConnection.h"
#include "net/connection/UdpConnection.h"
#include "net/connection/KcpConnection.h"

#include <iostream>


namespace saf
{
	std::atomic_int gConnectionCounter(0);

	Server::Server(EventLoop* loop, const InetAddress &addr, NetProtocal protocal):
			_running(false),
			_loop(loop),
			_acceptor(new Acceptor(_loop, addr)),
			_cluster(new EventLoopCluster())
	{

		LOG_INFO("Server(%p) was created", this);
	}

	Server::~Server()
	{
		assert(!_running);
		LOG_INFO("Server(%p) was destroied", this);
	}

	void Server::start(size_t threadCount)
	{
		if (_running)
			return;
		_running = true;

		_loop->runInLoop([this, threadCount]()
		{
			_cluster->start(threadCount);
			_acceptor->setAcceptCallback(
					std::bind(&Server::newConnectionInLoop, this, std::placeholders::_1, std::placeholders::_2));
			_acceptor->listenInLoop();

			LOG_INFO("Server(%p) is listening on %s", this, _acceptor->getAddress().toIpPort().c_str());
		});
	}

	void Server::stop()
	{
		_running = false;

		_loop->runInLoop([this]()
		{
			_cluster->stop();
			_acceptor->setAcceptCallback(nullptr);
			_acceptor->stopInLoop();

			for (auto& conn : _connections)
			{
				conn.second->setObserver(nullptr);
				conn.second->forceClose();
			}
			_connections.clear();

			LOG_INFO("Server(%p) was stopped on %s", this, _acceptor->getAddress().toIpPort().c_str());
		});
	}

	ConnectionPtr Server::createConnectionInLoop(int connfd)
	{
		int index = ++gConnectionCounter;
		Socket* socket = new Socket(connfd);

		switch (_protocal)
		{
			case NetProtocal::TCP:
				return ConnectionPtr(new TcpConnection(getLoop(), socket, index));
			case NetProtocal::UDP:
				return ConnectionPtr(new UdpConnection(getLoop(), socket, index));
			case NetProtocal::KCP:
				return ConnectionPtr(new KcpConnection(getLoop(), socket, index));
		}
	}

	void Server::newConnectionInLoop(int connfd, const InetAddress &addr)
	{
		auto looper = _cluster->getNextLoop();
		ConnectionPtr conn = createConnectionInLoop(connfd);
		_connections[conn->getIndex()] = conn;

		conn->setObserver(this);
		conn->getLooper()->runInLoop(std::bind(&Connection::onConnectEstablishedInLoop, conn));

		LOG_INFO("Server::newConnectionInLoop  connection(%d) from %s", conn->getIndex(), addr.toIpPort().c_str());
	}

	void Server::removeConnectionInLoop(const ConnectionPtr &conn)
	{
		_loop->assertInLoopThread();

		_connections.erase(conn->getIndex());
		conn->getLooper()->runInLoop(std::bind(&Connection::onConnectDestroyedInLoop, conn));

		LOG_INFO("Server::removeConnectionInLoop  connection(%d)", conn->getIndex());
	}

	void Server::onReceivedMessageInConnection(const ConnectionPtr& conn, Buffer* buffer)
	{
		if (_recvMessageCallback)
			_recvMessageCallback(conn, buffer);
		else
			conn->send(buffer->retrieveAsNetString());
	}

	void Server::onWriteCompletedInConnection(const ConnectionPtr& conn)
	{
		if (_writeCompleteCallback)
			_writeCompleteCallback(conn);
	}

	void Server::onConnectChangedInConnection(const ConnectionPtr& conn)
	{
		if (_connectChangeCallback)
			_connectChangeCallback(conn);
	}

	void Server::onClosedInConnection(const ConnectionPtr& conn)
	{
		_loop->queueInLoop([this, conn]()
		{
			this->removeConnectionInLoop(conn);
		});
	}

}