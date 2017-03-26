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
	std::atomic_int gConnectionIndex(1000);

	Client::Client(EventLoop* loop, const InetAddress &addr, NetProtocal protocal, float reconnectDelay) :
		_loop(loop),
		_reconnectDelay(reconnectDelay),
		_connector(new Connector(_loop, reconnectDelay)),
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
			_connection->handleCloseInLoop();
	}

	void Client::connect()
	{
		if (_connecting)
			return;
		_connecting = true;

		_loop->runInLoop([this]()
		{
			_connector->setConnectedCallback(
					std::bind(&Client::newConnectionInLoop, this, std::placeholders::_1));
			_connector->connect(_addr, _protocal);
		});
	}

	void Client::disconnect()
	{
		_connecting = false;

		_loop->runInLoop([this]()
		{
			_connector->disconnect();
			_connection->handleCloseInLoop();
		});
	}

	void Client::removeConnectionInLoop(const ConnectionPtr &conn)
	{
		_loop->assertInLoopThread();
		assert(conn->getLooper() == _loop);
		assert(conn == _connection);

		_connection.reset();
		conn->onConnectDestroyedInLoop();
	}

	bool Client::newConnectionInLoop(std::unique_ptr<Socket> &socketPtr)
	{
		int index = ++gConnectionIndex;
		Socket* socket = socketPtr.release();
		switch (_protocal)
		{
			case NetProtocal::TCP:
				_connection.reset(new TcpConnection(_loop, socket, index));
				break;
			case NetProtocal::UDP:
				_connection.reset(new UdpConnection(_loop, socket, index));
				break;
			case NetProtocal::KCP:
				_connection.reset(new KcpConnection(_loop, socket, index));
				break;
		}
		_connection->setObserver(this);
		_connection->onConnectEstablishedInLoop();
		return true;
	}

	void Client::onReceivedMessageInConnection(const ConnectionPtr& conn, Buffer* buffer)
	{
		if (_recvMessageCallback)
			_recvMessageCallback(conn, buffer);
		else
			buffer->retrieveAll();
	}

	void Client::onWriteCompletedInConnection(const ConnectionPtr& conn)
	{
		if (_writeCompleteCallback)
			_writeCompleteCallback(conn);
	}

	void Client::onConnectChangedInConnection(const ConnectionPtr& conn)
	{
		if (_connectChangeCallback)
			_connectChangeCallback(conn);
	}

	void Client::onClosedInConnection(const ConnectionPtr& conn)
	{
		// FIXME: remove self during event handling
		_loop->queueInLoop([this, conn]()
		{
			removeConnectionInLoop(conn);
			if (_reconnectDelay > 0)
			{
				_connector->connect(_addr, _protocal);
			}
		});
	}
}