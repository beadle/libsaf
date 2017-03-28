//
// Created by beadle on 3/19/17.
//

#include <unistd.h>
#include <iostream>
#include <string.h>
#include <map>

#include "TcpConnection.h"
#include "base/Logging.h"
#include "base/TimeUtils.h"
#include "net/fd/Socket.h"
#include "net/EventLoop.h"


namespace saf
{
	std::map<int, const char*> gStatusNames = {
			{TcpConnection::kConnected, "kConnected"},
			{TcpConnection::kConnecting, "kConnecting"},
			{TcpConnection::kDisconnected, "kDisconnected"},
			{TcpConnection::kDisconnecting, "kDisconnecting"}
	};

	TcpConnection::TcpConnection(
			EventLoop* loop, Socket* socket, const std::string& index, const InetAddress& addr) :
		Connection(loop, index, addr),
		_status(kConnecting),
		_socket(socket)
	{
		_socket->setReadCallback(std::bind(&TcpConnection::handleReadInLoop, this));
		_socket->setWriteCallback(std::bind(&TcpConnection::handleWriteInLoop, this));
		_socket->setCloseCallback(std::bind(&TcpConnection::handleCloseInLoop, this));
		_socket->setErrorCallback(std::bind(&TcpConnection::handleErrorInLoop, this));
	}

	TcpConnection::~TcpConnection()
	{
		assert(_status == kDisconnected);
	}

	void TcpConnection::forceClose()
	{
		if (_status != kConnected)
			return;

		changeStatus(kDisconnecting);

		auto self = shared_from_this();
		_loop->queueInLoop([self, this]()
		{
			handleCloseInLoop();
		});
	}

	void TcpConnection::shutdown()
	{
		if (_status != kConnected)
			return;

		changeStatus(kDisconnecting);

		auto self = shared_from_this();
		_loop->queueInLoop([self, this]()
		{
			if (!_socket->isWriting())
				_socket->shutdown();
		});
	}

	void TcpConnection::sendInLoop(const char *data, size_t len)
	{
		_loop->assertInLoopThread();

		_socket->enableWriteInLoop();
		_outputBuffer.append(data, len);
	}

	void TcpConnection::changeStatus(int status)
	{
		_status = status;
	}

	ssize_t TcpConnection::readInLoop()
	{
		return _inputBuffer.readFd(_socket->getFd());
	}

	ssize_t TcpConnection::writeInLoop(const char *buffer, size_t length)
	{
		return ::write(_socket->getFd(), buffer, length);
	}

	void TcpConnection::handleReadInLoop()
	{
		_loop->assertInLoopThread();

		ssize_t n = readInLoop();
		if (n > 0)
		{
			_activedTime = time::timestamp();
			if (_recvMessageCallback)
				_recvMessageCallback(shared_from_this(), &_inputBuffer);
		}
		else if (n == 0)
		{
			handleCloseInLoop();
		}
		else
		{
			LOG_ERROR("TcpConnection::handleReadInLoop");
			handleErrorInLoop();
		}
	}

	void TcpConnection::handleWriteInLoop()
	{
		_loop->assertInLoopThread();

		if (_socket->isWriting())
		{
			ssize_t n = writeInLoop(_outputBuffer.peek(), _outputBuffer.readableBytes());
			if (n > 0)
			{
				_outputBuffer.retrieve(static_cast<size_t >(n));
				if (_outputBuffer.readableBytes() == 0)
				{
					_socket->disableWriteInLoop();
					if (_writeCompleteCallback)
						_writeCompleteCallback(shared_from_this());
				}
			}
			else
			{
				LOG_ERROR("TcpConnection::handleWriteInLoop")
			}
		}
		else
		{
			LOG_DEBUG("TcpConnection(%d) is down, no more writing.", getIndex());
		}
	}

	void TcpConnection::handleCloseInLoop()
	{
		_loop->assertInLoopThread();

		LOG_DEBUG("TcpConnection(%d) state = %s", getIndex(), gStatusNames[_status]);

		assert(_status == kConnected || _status == kDisconnecting);

		_socket->detachInLoop();
		changeStatus(kDisconnected);

		if (_connectChangeCallback)
			_connectChangeCallback(shared_from_this());

		if (_closeCallback)
			_closeCallback(shared_from_this());
	}

	void TcpConnection::handleErrorInLoop()
	{
		_loop->assertInLoopThread();

		int err = _socket->getSocketError();
		LOG_ERROR("TcpConnection::handleErrorInLoop [%d] - SO_ERROR(%d): %s", getIndex(), err, errnoToString(err));
	}

	void TcpConnection::onConnectEstablishedInLoop()
	{
		_loop->assertInLoopThread();

		changeStatus(kConnected);
		_socket->attachInLoop(_loop);
		_socket->enableReadInLoop();

		if (_connectChangeCallback)
			_connectChangeCallback(shared_from_this());
	}

	void TcpConnection::onConnectDestroyedInLoop()
	{
		_loop->assertInLoopThread();

		/// redundant code
		_socket->detachInLoop();
		changeStatus(kDisconnected);
	}
}