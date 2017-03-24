//
// Created by beadle on 3/19/17.
//

#include "Server.h"
#include "Connection.h"

#include "base/Logging.h"
#include "net/EventLoop.h"
#include "net/EventLoopCluster.h"
#include "net/Acceptor.h"
#include "net/fd/Socket.h"

#include "connection/TcpConnection.h"
#include "connection/UdpConnection.h"
#include "connection/KcpConnection.h"

#include <iostream>


namespace saf
{
	Server::Server(const InetAddress &addr, NetProtocal protocal):
			_started(false),
			_connCount(0),
			_loop(new EventLoop()),
			_acceptor(new Acceptor(_loop.get(), addr)),
			_cluster(new EventLoopCluster())
	{
		_acceptor->setAcceptCallback(
				std::bind(&Server::newConnection, this, std::placeholders::_1, std::placeholders::_2));

		LOG_INFO("Server(%p) was created", this);
	}

	Server::~Server()
	{
		stop();

		for (auto& conn : _connections)
		{
			conn.second->setObserver(nullptr);
			conn.second->forceClose();
		}

		LOG_INFO("Server(%p) was destroied", this);
	}

	void Server::start(size_t threadCount)
	{
		if (_started)
			return;

		_loop->runInLoop([this]() {
			_acceptor->listen();
			LOG_INFO("Server(%p) is listening on %s", this, _acceptor->getAddress().toIpPort().c_str());
		});

		_cluster->start(threadCount);
		_loop->start();
	}

	void Server::stop()
	{
		_loop->stop();
		_cluster->stop();
	}

	ConnectionPtr Server::createConnection(int connfd)
	{
		int index = ++_connCount;
		Socket* socket = Socket::create(
				_loop.get(), _protocal, _acceptor->getAddress().getFamily());

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

	void Server::newConnection(int connfd, const InetAddress &addr)
	{
		auto looper = _cluster->getNextLoop();
		ConnectionPtr conn = createConnection(connfd);
		_connections[conn->getIndex()] = conn;

		LOG_INFO("Server::newConnection  connection(%d) from %s", conn->getIndex(), addr.toIpPort().c_str());

		conn->setObserver(this);
		conn->getLooper()->runInLoop(std::bind(&Connection::onConnectEstablished, conn));
	}

	void Server::removeConnection(const ConnectionPtr &conn)
	{
		_loop->assertInLoopThread();

		LOG_INFO("Server::removeConnection  connection(%d)", conn->getIndex());

		_connections.erase(conn->getIndex());
		conn->getLooper()->runInLoop(std::bind(&Connection::onConnectDestroyed, conn));
	}

	void Server::onConnReceivedMessage(const ConnectionPtr& conn, Buffer* buffer)
	{
		if (_recvMessageCallback)
			_recvMessageCallback(conn, buffer);
		else
			conn->send(buffer->retrieveAsNetString());
	}

	void Server::onConnWriteCompleted(const ConnectionPtr& conn)
	{
		if (_writeCompleteCallback)
			_writeCompleteCallback(conn);
	}

	void Server::onConnConnectChanged(const ConnectionPtr& conn)
	{
		if (_connectChangeCallback)
			_connectChangeCallback(conn);
	}

	void Server::onConnClosed(const ConnectionPtr& conn)
	{
		_loop->queueInLoop([this, conn](){
			this->removeConnection(conn);
		});
	}

}