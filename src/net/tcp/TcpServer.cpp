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
	std::atomic_int gConnectionCounter(0);

	TcpServer::TcpServer(EventLoop* loop, const InetAddress &addr):
			_running(false),
			_loop(loop),
			_acceptor(new TcpAcceptor(_loop, addr)),
			_cluster(new EventLoopCluster())
	{

		LOG_INFO("TcpServer(%p) was created", this);
	}

	TcpServer::~TcpServer()
	{
		assert(!_running);
		LOG_INFO("TcpServer(%p) was destroied", this);
	}

	void TcpServer::start(size_t threadCount)
	{
		if (_running)
			return;
		_running = true;

		_loop->runInLoop([this, threadCount]()
		{
			_cluster->start(threadCount);
			_acceptor->setAcceptCallback(
					std::bind(&TcpServer::newConnectionInLoop, this, std::placeholders::_1, std::placeholders::_2));
			_acceptor->listenInLoop();

			LOG_INFO("TcpServer(%p) is listening on %s", this, _acceptor->getAddress().toIpPort().c_str());
		});
	}

	void TcpServer::stop()
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

			LOG_INFO("TcpServer(%p) was stopped on %s", this, _acceptor->getAddress().toIpPort().c_str());
		});
	}

	ConnectionPtr TcpServer::createConnectionInLoop(int connfd, const InetAddress& addr)
	{
		int index = ++gConnectionCounter;
		Socket* socket = new Socket(connfd);
		return ConnectionPtr(new TcpConnection(getLoop(), socket, index));
	}

	void TcpServer::newConnectionInLoop(int connfd, const InetAddress &addr)
	{
		auto looper = _cluster->getNextLoop();
		ConnectionPtr conn = createConnectionInLoop(connfd, addr);
		_connections[conn->getIndex()] = conn;

		conn->setObserver(this);
		conn->getLooper()->runInLoop(std::bind(&TcpConnection::onConnectEstablishedInLoop, conn));

		LOG_INFO("TcpServer::newConnectionInLoop  connection(%d) from %s", conn->getIndex(), addr.toIpPort().c_str());
	}

	void TcpServer::removeConnectionInLoop(const ConnectionPtr &conn)
	{
		_loop->assertInLoopThread();

		_connections.erase(conn->getIndex());
		conn->getLooper()->runInLoop(std::bind(&TcpConnection::onConnectDestroyedInLoop, conn));

		LOG_INFO("TcpServer::removeConnectionInLoop  connection(%d)", conn->getIndex());
	}

	void TcpServer::onReceivedMessageInConnection(const ConnectionPtr& conn, Buffer* buffer)
	{
		if (_recvMessageCallback)
			_recvMessageCallback(conn, buffer);
		else
			conn->send(buffer->retrieveAsNetString());
	}

	void TcpServer::onWriteCompletedInConnection(const ConnectionPtr& conn)
	{
		if (_writeCompleteCallback)
			_writeCompleteCallback(conn);
	}

	void TcpServer::onConnectChangedInConnection(const ConnectionPtr& conn)
	{
		if (_connectChangeCallback)
			_connectChangeCallback(conn);
	}

	void TcpServer::onClosedInConnection(const ConnectionPtr& conn)
	{
		_loop->queueInLoop([this, conn]()
		{
			this->removeConnectionInLoop(conn);
		});
	}

}