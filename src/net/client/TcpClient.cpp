//
// Created by beadle on 3/24/17.
//

#include "TcpClient.h"
#include "TcpConnector.h"

#include "net/connection/TcpConnection.h"
#include "net/EventLoop.h"


namespace saf
{
	std::atomic_int gConnectionIndex(1000);

	TcpClient::TcpClient(EventLoop* loop, const InetAddress &addr, float reconnectDelay) :
		_loop(loop),
		_reconnectDelay(reconnectDelay),
		_connector(new TcpConnector(_loop, reconnectDelay)),
		_addr(addr),
		_connecting(false)
	{

	}

	TcpClient::~TcpClient()
	{
		_connector->disconnect();
		_connector.reset();

		if (_connection)
			_connection->handleCloseInLoop();
	}

	void TcpClient::connect()
	{
		if (_connecting)
			return;
		_connecting = true;

		_loop->runInLoop([this]()
		{
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

	void TcpClient::removeConnectionInLoop(const ConnectionPtr &conn)
	{
		_loop->assertInLoopThread();
		assert(conn->getLooper() == _loop);
		assert(conn == _connection);

		_connection.reset();
		conn->onConnectDestroyedInLoop();
	}

	bool TcpClient::newConnectionInLoop(std::unique_ptr<Socket> &socket)
	{
		_connection.reset(
				new TcpConnection(_loop, socket.release(), ++gConnectionIndex));
		_connection->setObserver(this);
		_connection->onConnectEstablishedInLoop();
		return true;
	}

	void TcpClient::onReceivedMessageInConnection(const ConnectionPtr& conn, Buffer* buffer)
	{
		if (_recvMessageCallback)
			_recvMessageCallback(conn, buffer);
		else
			buffer->retrieveAll();
	}

	void TcpClient::onWriteCompletedInConnection(const ConnectionPtr& conn)
	{
		if (_writeCompleteCallback)
			_writeCompleteCallback(conn);
	}

	void TcpClient::onConnectChangedInConnection(const ConnectionPtr& conn)
	{
		if (_connectChangeCallback)
			_connectChangeCallback(conn);
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