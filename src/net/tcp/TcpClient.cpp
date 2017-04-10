//
// Created by beadle on 3/24/17.
//
#include <iostream>

#include "TcpClient.h"
#include "TcpConnector.h"

#include "TcpConnection.h"
#include "net/EventLoop.h"
#include "net/fd/Socket.h"


namespace saf
{
	std::atomic_int gConnectionIndex(1000);

	TcpClient::TcpClient(EventLoop* loop) :
		Client(loop),
		_connector(new TcpConnector(loop, this))
	{

	}

	TcpClient::~TcpClient()
	{
		assert(!_connecting);
	}

	bool TcpClient::isConnected() const
	{
		return _connection && _connection->isConnected();
	}

	void TcpClient::connect(const InetAddress& addr)
	{
		if (_connecting)
			return;
		_connecting = true;

		_loop->runInLoop([this, addr]()
		{
			_addr = addr;
			_connector->setConnectedCallback(
					std::bind(&TcpClient::newConnectionInLoop, this, std::placeholders::_1));
			_connector->connect(_addr);
		});
	}

	void TcpClient::disconnect()
	{
		_connecting = false;

		auto pin = shared_from_this();
		_loop->runInLoop([this, pin]()
		{
			if (_connector)
			{
				_connector->disconnect();
			}

			if (_connection)
			{
				_connection->close();
				_connection.reset();
			}
		});
	}

	ConnectionPtr TcpClient::getConnection() const
	{
		return _connection;
	}

	void TcpClient::removeConnectionInLoop(const ConnectionPtr &conn)
	{
		_loop->assertInLoopThread();
		assert(conn->getLooper() == _loop);
		assert(conn.get() == _connection.get());

		notifyConnectionDestroyed(conn);
		_connection.reset();
	}

	bool TcpClient::newConnectionInLoop(std::unique_ptr<Socket> &socket)
	{
		socket->setObserver(nullptr);
		_connection.reset(new TcpConnection(_loop, socket.release(), _addr.toIpPort(), _addr));

		notifyConnectionEstablished(std::dynamic_pointer_cast<Connection>(_connection));
		return true;
	}

	void TcpClient::onClosedInConnection(const ConnectionPtr& conn)
	{
		_loop->runInLoop([this, conn]()
		{
			removeConnectionInLoop(conn);
			if (_connecting)
			{
				if (_reconnectDelay > 0)
				{
					_connector->connect(_addr);
				}
			}
		});
	}
}