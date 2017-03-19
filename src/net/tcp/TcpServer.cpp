//
// Created by beadle on 3/19/17.
//

#include "TcpServer.h"
#include "TcpConnection.h"

#include "base/Logging.h"
#include "net/EventLoop.h"
#include "net/Acceptor.h"
#include "net/fd/Socket.h"


namespace saf
{
	TcpServer::TcpServer(EventLoop* loop, const InetAddress &addr):
		_started(false),
		_connCount(0),
		_loop(loop),
		_acceptor(new Acceptor(loop, addr))
	{
		_acceptor->setAcceptCallback(
				std::bind(&TcpServer::handleAccept, this, std::placeholders::_1, std::placeholders::_2));
	}

	TcpServer::~TcpServer()
	{
		for (auto it = _connMap.begin(); it != _connMap.end(); ++it)
		{
			// TODO: connect destroyed callback
		}
	}

	void TcpServer::start()
	{
		if (_started)
			return;

		_loop->runInLoop([this](){
			_acceptor->listen();
		});
	}

	void TcpServer::handleAccept(int connfd, const InetAddress &addr)
	{
		auto index = ++_connCount;
		ConnPtr ptr(new TcpConnection(_loop, connfd, index));
		_connMap[index] = std::move(ptr);
		LOG_INFO("TcpServer::newConnection - new connection");
	}

}