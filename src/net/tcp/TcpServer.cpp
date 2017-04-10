//
// Created by beadle on 3/19/17.
//

#include "TcpServer.h"
#include "TcpConnection.h"

#include "base/Logging.h"
#include "net/EventLoop.h"
#include "net/EventLoopCluster.h"
#include "TcpAcceptor.h"
#include "net/fd/Socket.h"

#include <iostream>


namespace saf
{
	TcpServer::TcpServer(EventLoop* loop):
		Server(loop),
		_acceptor(new TcpAcceptor(_loop))
	{
		LOG_INFO("TcpServer(%p) was created", this);
	}

	TcpServer::~TcpServer()
	{
		assert(!_running);
		_cluster->stop();

		LOG_INFO("TcpServer(%p) was destroied", this);
	}

	void TcpServer::start(const InetAddress& addr, size_t threadCount)
	{
		if (_running)
			return;
		_running = true;

		_loop->runInLoop([this, threadCount, addr]()
		{
			_cluster->start(threadCount);
			_acceptor->setAcceptCallback(
					std::bind(&TcpServer::newConnectionInLoop, this, std::placeholders::_1, std::placeholders::_2));
			_acceptor->listenInLoop(addr, true);

			LOG_INFO("TcpServer(%p) is listening on %s", this, _acceptor->getAddress().toIpPort().c_str());
		});
	}

	void TcpServer::stop()
	{
		_running = false;

		auto pin = shared_from_this();
		_loop->runInLoop([this, pin]()
		{
			_acceptor->setAcceptCallback(nullptr);
			_acceptor->stopInLoop();

			auto deleted = _connections;
			for (auto& conn : deleted)
			{
				removeConnectionInLoop(conn.second);
				conn.second->close();
			}
			_connections.clear();

			LOG_INFO("TcpServer(%p) was stopped on %s", this, _acceptor->getAddress().toIpPort().c_str());
		});
	}

	TcpServer::TcpConnectionPtr TcpServer::createConnectionInLoop(int connfd, const InetAddress& addr)
	{
		Socket* socket = new Socket(connfd, NetProtocal::TCP);
		return TcpConnectionPtr(new TcpConnection(_cluster->getNextLoop(), socket, addr.toIpPort(), addr));
	}

	void TcpServer::newConnectionInLoop(int connfd, const InetAddress &addr)
	{
		_loop->assertInLoopThread();

		TcpConnectionPtr conn = createConnectionInLoop(connfd, addr);
		_connections[conn->getIndex()] = conn;

		this->notifyConnectionEstablished(conn);
	}

	void TcpServer::removeConnectionInLoop(const ConnectionPtr &conn)
	{
		_loop->assertInLoopThread();

		auto it = _connections.find(conn->getIndex());
		if (it != _connections.end())
			_connections.erase(it);

		this->notifyConnectionDestroyed(conn);
	}

	void TcpServer::onClosedInConnection(const ConnectionPtr& conn)
	{
		auto pin = shared_from_this();
		_loop->queueInLoop([this, conn, pin]()
		{
			this->removeConnectionInLoop(conn);
		});
	}

}