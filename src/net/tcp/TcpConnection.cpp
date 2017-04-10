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
		_socket->setObserver(this);
	}

	TcpConnection::~TcpConnection()
	{
		assert(_status == kDisconnected);
		if (_socket)
			delete _socket;
	}

	void TcpConnection::setTcpNoDelay(bool on)
	{
		_socket->setTcpNoDelay(on);
	}

	void TcpConnection::close()
	{
		if (_status != kConnected)
			return;

		changeStatus(kDisconnecting);

		auto self = shared_from_this();
		_loop->runInLoop([self, this]()
		{
			closeInLoop();
		});
	}

	void TcpConnection::shutdown()
	{
		if (_status != kConnected)
			return;

		changeStatus(kDisconnecting);

		auto self = shared_from_this();
		_loop->runInLoop([self, this]()
		{
			if (!_socket->isWriting())
				_socket->shutdown();
		});
	}

	void TcpConnection::closeInLoop()
	{
		_loop->assertInLoopThread();

		_socket->detachInLoop();
		changeStatus(kDisconnected);

		if (_connectChangeCallback)
			_connectChangeCallback(shared_from_this());

		if (_closeCallback)
			_closeCallback(shared_from_this());
	}

	void TcpConnection::sendInLoop(const char *data, size_t len)
	{
		_loop->assertInLoopThread();

		ssize_t nwrote = 0;
		size_t remaining = len;
		bool faultError = false;

		if (!_socket->isWriting() && !_outputBuffer.readableBytes())
		{
			nwrote = writeInLoop(data, len);
			if (nwrote >= 0)
			{
				remaining = len - nwrote;
				if (!remaining)
				{
					if (_writeCompleteCallback)
						_loop->queueInLoop(std::bind(_writeCompleteCallback, shared_from_this()));
					return;
				}
			}
			else
			{
				nwrote = 0;
				if (errno != EWOULDBLOCK)
				{
					LOG_ERROR("TcpConnection::sendInLoop");
					if (errno == EPIPE || errno == ECONNRESET) // FIXME: any others?
					{
						faultError = true;
					}
				}
			}
		}

		assert(remaining <= len);
		if (!faultError && remaining > 0)
		{
			_outputBuffer.append(data + nwrote, remaining);
			if (!_socket->isWriting())
			{
				_socket->enableWriteInLoop();
			}
		}
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
				_socket->disableWriteInLoop();
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

		closeInLoop();
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

	void TcpConnection::onReadInIOFd(IOFd*)
	{
		handleReadInLoop();
	}

	void TcpConnection::onWriteInIOFd(IOFd*)
	{
		handleWriteInLoop();
	}

	void TcpConnection::onErrorInIOFd(IOFd*)
	{
		handleErrorInLoop();
	}

	void TcpConnection::onCloseInIOFd(IOFd*)
	{
		handleCloseInLoop();
	}

}