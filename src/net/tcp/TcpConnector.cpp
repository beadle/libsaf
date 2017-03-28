//
// Created by beadle on 3/24/17.
//
#include <assert.h>
#include <unistd.h>

#include "TcpConnector.h"
#include "TcpClient.h"

#include "base/Logging.h"
#include "net/EventLoop.h"
#include "net/fd/Socket.h"
#include "TcpConnection.h"


namespace saf
{
	TcpConnector::TcpConnector(EventLoop* loop, Client* master) :
		_loop(loop),
		_stopping(false),
		_status(kDisconnected),
		_master(master)
	{

	}

	TcpConnector::~TcpConnector()
	{

	}

	void TcpConnector::connect(const InetAddress &addr)
	{
		_addr = addr;
		_stopping = false;
		_loop->runInLoop(std::bind(&TcpConnector::connectInLoop, this));
	}

	void TcpConnector::disconnect()
	{
		_stopping = true;
		_loop->runInLoop(std::bind(&TcpConnector::disconnectInLoop, this));
	}

	void TcpConnector::connectInLoop()
	{
		_loop->assertInLoopThread();
		if (_stopping) return;

		resetSocket();

		_socket.reset(Socket::create(NetProtocal::TCP, _addr.getFamily()));
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
				LOG_ERROR("connect error(%d) in TcpConnector::connectInLoop", savedErrno);
				_socket.reset();
				break;

			default:
				LOG_ERROR("connect error(%d) in TcpConnector::connectInLoop", savedErrno);
				_socket.reset();
				break;
		}
	}

	void TcpConnector::disconnectInLoop()
	{
		_loop->assertInLoopThread();

		changeStatus(kDisconnected);
		resetSocket();
	}

	void TcpConnector::onConnectingInLoop()
	{
		changeStatus(kConnecting);

		_socket->setReadCallback(nullptr);
		_socket->setWriteCallback(std::bind(&TcpConnector::handleWriteInLoop, this));
		_socket->setErrorCallback(std::bind(&TcpConnector::handleErrorInLoop, this));
		_socket->setCloseCallback(std::bind(&TcpConnector::handleCloseInLoop, this));
		_socket->attachInLoop(_loop);
		_socket->enableWriteInLoop();
	}

	void TcpConnector::onRetryInLoop()
	{
		changeStatus(kDisconnected);
		resetSocket();

		float retrySeconds = _master->getReconnectDelay();
		if (retrySeconds > 0)
		{
			LOG_INFO("TcpConnector::onRetryInLoop - Retry connecting to %s in %f seconds.", _addr.toIpPort().c_str(), retrySeconds);
			_loop->addTimer(retrySeconds, std::bind(&TcpConnector::connectInLoop, shared_from_this()));
		}
	}

	void TcpConnector::handleWriteInLoop()
	{
		if (_status == kConnecting)
		{
			_socket->detachInLoop();
			int err = _socket->getSocketError();
			if (err)
			{
				LOG_WARN("TcpConnector::handleWriteInLoop - SO_ERROR(%d): %s", err, errnoToString(err));
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
			LOG_WARN("TcpConnector::handleWriteInLoop error status: %d", int(_status));
			changeStatus(kDisconnected);
		}
	}

	void TcpConnector::handleErrorInLoop()
	{
		LOG_ERROR("TcpConnector::handleErrorInLoop error status: %d", int(_status));
		if (_status == kConnecting)
		{
			int err = _socket->getSocketError();
			LOG_ERROR("TcpConnector::handleErrorInLoop - SO_ERROR(%d): %s", err, errnoToString(err));

			_loop->queueInLoop([this](){
				onRetryInLoop();
			});
		}
	}

	void TcpConnector::handleCloseInLoop()
	{
		LOG_INFO("TcpConnector::handleCloseInLoop - status: %d", int(_status))
		if (_status == kConnecting)
		{
			_loop->queueInLoop([this](){
				onRetryInLoop();
			});
		}
	}

	void TcpConnector::resetSocket()
	{
		if (_socket)
		{
			_socket->detachInLoop();
			_socket.reset();
		}
	}
}