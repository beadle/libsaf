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

	void Connector::reconnect()
	{
		_stopping = false;
		_loop->runInLoop(std::bind(&Connector::reconnectInLoop, this));
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

		if (_socket)
		{
			detachFromEventPool();
			_socket.reset();
		}

		_socket.reset(Socket::create(_loop, _protocal, _addr.getFamily()));
		int ret = _socket->connect(_addr);
		int savedErrno = (ret == 0) ? 0 : errno;
		switch (savedErrno)
		{
			case 0:
			case EINPROGRESS:
			case EINTR:
			case EISCONN:
				onConnecting();
				break;

			case EAGAIN:
			case EADDRINUSE:
			case EADDRNOTAVAIL:
			case ECONNREFUSED:
			case ENETUNREACH:
				onRetry();
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

	void Connector::reconnectInLoop()
	{
		connectInLoop();
	}

	void Connector::disconnectInLoop()
	{
		_loop->assertInLoopThread();

		changeStatus(kDisconnected);
		detachFromEventPool();
		_socket.reset();
	}

	void Connector::onConnecting()
	{
		changeStatus(kConnecting);
		_socket->setWriteCallback(std::bind(&Connector::handleWrite, this));
		_socket->setErrorCallback(std::bind(&Connector::handleError, this));
		_socket->setReadCallback(nullptr);
		_socket->setCloseCallback(nullptr);
		_socket->enableWrite();
	}

	void Connector::onRetry()
	{
		changeStatus(kDisconnected);
		detachFromEventPool();
		_socket.reset();

		LOG_INFO("Connector::onRetry - Retry connecting to %s in %f seconds.", _addr.toIpPort(), _retrySeconds);
		_loop->addTimer(_retrySeconds, std::bind(&Connector::connectInLoop, shared_from_this()));
	}

	void Connector::handleWrite()
	{
		if (_status == kConnecting)
		{
			detachFromEventPool();
			int err = _socket->getSocketError();
			if (err)
			{
				LOG_WARN("Connector::handleWrite - SO_ERROR(%d): %s", err, errnoToString(err));
				onRetry();
			}
			else
			{
				changeStatus(kConnected);
				if (_connectedCallback)
					_connectedCallback(_socket);
			}
		}
		else
		{
			detachFromEventPool();
			LOG_WARN("Connector::handleWrite error status: %d", int(_status));
			changeStatus(kDisconnected);
		}
	}

	void Connector::handleError()
	{
		LOG_ERROR("Connector::handleError error status: %d", int(_status));
		if (_status == kConnecting)
		{
			int err = _socket->getSocketError();
			LOG_ERROR("onnector::handleError - SO_ERROR(%d): %s", err, errnoToString(err));

			_loop->queueInLoop([this](){
				onRetry();
			});
		}
	}

	void Connector::detachFromEventPool()
	{
		if (_socket)
		{
			_socket->disableAll();
			_loop->removeFd(_socket.get());
		}
	}
}