//
// Created by beadle on 3/24/17.
//
#include <iostream>

#include "TcpClient.h"
#include "TcpConnector.h"

#include "TcpConnection.h"
#include "net/EventLoop.h"


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
		_connector->disconnect();
		_connector.reset();

		if (_connection)
			_connection->handleCloseInLoop();
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

		_loop->runInLoop([this]()
		{
			_connector->disconnect();
			_connection->handleCloseInLoop();
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
		_connection.reset(new TcpConnection(_loop, socket.release(), _addr.toIpPort(), _addr));
		std::cout << "=================" << _connection->getAddress().toIpPort().c_str();
		notifyConnectionEstablished(std::dynamic_pointer_cast<Connection>(_connection));
		return true;
	}

	void TcpClient::onClosedInConnection(const ConnectionPtr& conn)
	{
		// FIXME: remove self during event handling
		_loop->queueInLoop([this, conn]()
		{
			removeConnectionInLoop(conn);
			if (_reconnectDelay > 0)
			{
				_connector->connect(_addr);
			}
		});
	}
}