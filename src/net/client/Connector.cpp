//
// Created by beadle on 3/24/17.
//
#include <assert.h>
#include <unistd.h>

#include "Connector.h"

#include "base/Logging.h"
#include "net/EventLoop.h"
#include "net/fd/Socket.h"
#include "net/Connection.h"
#include "net/connection/TcpConnection.h"
#include "net/connection/KcpConnection.h"
#include "net/connection/UdpConnection.h"


namespace saf
{
	Connector::Connector(EventLoop* loop, float retrySeconds) :
		_loop(loop),
		_stopping(false),
		_status(kDisconnected),
		_retrySeconds(retrySeconds)
	{

	}

	Connector::~Connector()
	{

	}

	void Connector::connect(const InetAddress &addr, NetProtocal protocal)
	{
		_addr = addr;
		_protocal = protocal;
		_stopping = false;
		_loop->runInLoop(std::bind(&Connector::connectInLoop, this));
	}

	void Connector::disconnect()
	{
		_stopping = true;
		_loop->runInLoop(std::bind(&Connector::disconnectInLoop, this));
	}

	void Connector::connectInLoop()
	{
		_loop->assertInLoopThread();
		if (_stopping) return;

		resetSocket();

		_socket.reset(Socket::create(_protocal, _addr.getFamily()));
		int ret = _socket->connect(_addr);
		int savedErrno = (ret == 0) ? 0 : errno;
		switch (savedErrno)
		{
			case 0:
			case EINPROGRESS:
			case EINTR:
			case EISCONN:
				onConnectingInLoop();
				break;

			case EAGAIN:
			case EADDRINUSE:
			case EADDRNOTAVAIL:
			case ECONNREFUSED:
			case ENETUNREACH:
				onRetryInLoop();
				break;

			case EACCES:
			case EPERM:
			case EAFNOSUPPORT:
			case EALREADY:
			case EBADF:
			case EFAULT:
			case ENOTSOCK:
				LOG_ERROR("connect error(%d) in Connector::connectInLoop", savedErrno);
				_socket.reset();
				break;

			default:
				LOG_ERROR("connect error(%d) in Connector::connectInLoop", savedErrno);
				_socket.reset();
				break;
		}
	}

	void Connector::disconnectInLoop()
	{
		_loop->assertInLoopThread();

		changeStatus(kDisconnected);
		resetSocket();
	}

	void Connector::onConnectingInLoop()
	{
		changeStatus(kConnecting);

		_socket->setWriteCallback(std::bind(&Connector::handleWriteInLoop, this));
		_socket->setErrorCallback(std::bind(&Connector::handleErrorInLoop, this));
		_socket->setReadCallback(nullptr);
		_socket->setCloseCallback(std::bind(&Connector::handleCloseInLoop, this));
		_socket->attachInLoop(_loop);
		_socket->enableWriteInLoop();
	}

	void Connector::onRetryInLoop()
	{
		changeStatus(kDisconnected);
		resetSocket();

		LOG_INFO("Connector::onRetryInLoop - Retry connecting to %s in %f seconds.", _addr.toIpPort().c_str(), _retrySeconds);
		_loop->addTimer(_retrySeconds, std::bind(&Connector::connectInLoop, shared_from_this()));
	}

	void Connector::handleWriteInLoop()
	{
		if (_status == kConnecting)
		{
			_socket->detachInLoop();
			int err = _socket->getSocketError();
			if (err)
			{
				LOG_WARN("Connector::handleWriteInLoop - SO_ERROR(%d): %s", err, errnoToString(err));
				onRetryInLoop();
			}
			else
			{
				LOG_INFO("Connect to %s successfully.", _addr.toIpPort().c_str());
				changeStatus(kConnected);
				if (_connectedCallback)
					_connectedCallback(_socket);
			}
		}
		else
		{
			LOG_WARN("Connector::handleWriteInLoop error status: %d", int(_status));
			changeStatus(kDisconnected);
		}
	}

	void Connector::handleErrorInLoop()
	{
		LOG_ERROR("Connector::handleErrorInLoop error status: %d", int(_status));
		if (_status == kConnecting)
		{
			int err = _socket->getSocketError();
			LOG_ERROR("Connector::handleErrorInLoop - SO_ERROR(%d): %s", err, errnoToString(err));

			_loop->queueInLoop([this](){
				onRetryInLoop();
			});
		}
	}

	void Connector::handleCloseInLoop()
	{
		LOG_INFO("Connector::handleCloseInLoop - status: %d", int(_status))
		if (_status == kConnecting)
		{
			_loop->queueInLoop([this](){
				onRetryInLoop();
			});
		}
	}

	void Connector::resetSocket()
	{
		if (_socket)
		{
			_socket->detachInLoop();
			_socket.reset();
		}
	}
}