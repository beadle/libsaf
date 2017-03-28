//
// Created by beadle on 3/27/17.
//
#include <iostream>

#include "UdpServer.h"
#include "UdpAcceptor.h"
#include "UdpConnection.h"

#include "net/fd/Socket.h"
#include "net/EventLoop.h"
#include "net/EventLoopCluster.h"
#include "base/Logging.h"
#include "base/TimeUtils.h"


namespace saf
{
	const long ExpiredTime = 30 * 1000;
	const float ExpiredInterval = 5.3;

	UdpServer::UdpServer(EventLoop *loop):
		Server(loop),
		_expiredChecker(0),
		_acceptor(new UdpAcceptor(loop))

	{
		LOG_INFO("UdpServer(%p) was created", this);
	}

	UdpServer::~UdpServer()
	{
		assert(!_running);
		LOG_INFO("UdpServer(%p) was destroied", this);
	}

	void UdpServer::start(const InetAddress& addr, size_t threadCount)
	{
		if (_running)
			return;
		_running = true;

		_loop->runInLoop([this, threadCount, addr]()
		 {
			 _cluster->start(threadCount);
			 _acceptor->setRecvMessageCallback(
					 std::bind(&UdpServer::recvMessageInLoop, this, std::placeholders::_1, std::placeholders::_2));
			 _acceptor->listenInLoop(addr, true);

			 LOG_INFO("UdpServer(%p) is listening on %s", this, _acceptor->getAddress().toIpPort().c_str());

			 _expiredChecker = _loop->addTimer(ExpiredInterval, [this]()
			 {
				 this->checkExpiredInLoop();
			 }, true);
		 });
	}

	void UdpServer::stop()
	{
		_running = false;

		_loop->runInLoop([this]()
		 {
			 _cluster->stop();
			 _acceptor->setRecvMessageCallback(nullptr);
			 _acceptor->stopInLoop();

			 // TODO: Clear All Connections

			 if (_expiredChecker)
				 _loop->cancelTimer(_expiredChecker);

			 LOG_INFO("UdpServer(%p) was stopped on %s", this, _acceptor->getAddress().toIpPort().c_str());
		 });
	}

	void UdpServer::recvMessageInLoop(InetAddress &addr, Buffer *buffer)
	{
		auto key = addr.toIpPort();
		auto it = _connections.find(key);
		UdpConnectionPtr conn(nullptr);

		if (it == _connections.end())
		{
			EventLoop* loop = _cluster->getNextLoop();
			conn = UdpConnectionPtr(new UdpConnection(loop, key, addr));
			_connections[key] = conn;

			this->notifyConnectionEstablished(conn);
		}
		else
		{
			conn = it->second;
		}

		std::shared_ptr<std::string> copied (new std::string(buffer->retrieveAllAsString()));
		conn->getLooper()->queueInLoop([conn, copied]()
		{
			conn->handleReadInLoop(copied->c_str(), copied->size());
		});
	}

	void UdpServer::checkExpiredInLoop()
	{
		auto now = time::timestamp();
		for (auto it = _connections.begin(); it != _connections.end();)
		{
			if (now - it->second->getActivedTime() > ExpiredTime)
			{
				UdpConnectionPtr conn = it->second;
				it = _connections.erase(it);

				this->notifyConnectionDestroyed(conn);
			}
			else
			{
				++it;
			}
		}
	}

	void UdpServer::onClosedInConnection(const ConnectionPtr& conn)
	{

	}

	void UdpServer::bindDefaultCallbacks(Connection* conn)
	{
		Server::bindDefaultCallbacks(conn);
		(static_cast<UdpConnection*>(conn))->setSenderFd(_acceptor->getSocket()->getFd());
	}

	void UdpServer::unbindDefaultCallbacks(Connection *conn)
	{
		Server::unbindDefaultCallbacks(conn);
		(static_cast<UdpConnection*>(conn))->setSenderFd(0);
	}

}