//
// Created by beadle on 3/24/17.
//

#include "Client.h"
#include "Connector.h"

#include "net/Connection.h"
#include "net/EventLoop.h"
#include "net/connection/TcpConnection.h"
#include "net/connection/KcpConnection.h"
#include "net/connection/UdpConnection.h"


namespace saf
{
	std::atomic_int gConnectionIndex(0);

	Client::Client(const InetAddress &addr, NetProtocal protocal, float reconnectDelay) :
		_loop(new EventLoop()),
		_reconnectDelay(_reconnectDelay),
		_connector(new Connector(_loop.get(), reconnectDelay)),
		_addr(addr),
		_protocal(protocal),
		_connecting(false)
	{

	}

	Client::~Client()
	{
		_connector->disconnect();
		_connector.reset();

		if (_connection)
			_connection->handleClose();
	}

	void Client::connect()
	{
		assert(!_connecting);

		_connector->setConnectedCallback(
				std::bind(&Client::onConnectedCallback, this, std::placeholders::_1));
		_connector->connect(_addr, _protocal);

		_loop->start();
	}

	void Client::disconnect()
	{
		_connecting = false;
		_connector->disconnect();
		_connection->handleClose();
	}

	void Client::removeConnection(const ConnectionPtr &conn)
	{
		_loop->assertInLoopThread();
		assert(conn->getLooper() == _loop.get());
		assert(conn == _connection);

		_connection.reset();
		conn->onConnectDestroyed();
	}

	bool Client::onConnectedCallback(std::unique_ptr<Socket>& socket)
	{
		int index = ++gConnectionIndex;
		switch (_protocal)
		{
			case NetProtocal::TCP:
				_connection.reset(new TcpConnection(_loop.get(), socket.release(), index));
				break;
			case NetProtocal::UDP:
				_connection.reset(new UdpConnection(_loop.get(), socket.release(), index));
				break;
			case NetProtocal::KCP:
				_connection.reset(new KcpConnection(_loop.get(), socket.release(), index));
				break;
		}
		_connection->setObserver(this);
		_connection->onConnectEstablished();
		return true;
	}

	void Client::onConnReceivedMessage(const ConnectionPtr& conn, Buffer* buffer)
	{
		if (_recvMessageCallback)
			_recvMessageCallback(conn, buffer);
		else
			buffer->retrieveAll();
	}

	void Client::onConnWriteCompleted(const ConnectionPtr& conn)
	{
		if (_writeCompleteCallback)
			_writeCompleteCallback(conn);
	}

	void Client::onConnConnectChanged(const ConnectionPtr& conn)
	{
		if (_connectChangeCallback)
			_connectChangeCallback(conn);
	}

	void Client::onConnClosed(const ConnectionPtr& conn)
	{
		removeConnection(conn);
		if (_reconnectDelay > 0)
		{
			_connector->reconnect();
		}
	}
}