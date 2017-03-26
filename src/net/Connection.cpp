//
// Created by beadle on 3/19/17.
//

#include <unistd.h>
#include <iostream>
#include <string.h>
#include <map>

#include "Connection.h"
#include "base/Logging.h"
#include "net/fd/Socket.h"
#include "net/EventLoop.h"


namespace saf
{
	std::map<int, const char*> gStatusNames = {
			{Connection::kConnected, "kConnected"},
			{Connection::kConnecting, "kConnecting"},
			{Connection::kDisconnected, "kDisconnected"},
			{Connection::kDisconnecting, "kDisconnecting"}
	};

	Connection::Connection(EventLoop* loop, Socket* socket, int index) :
		_index(index),
		_loop(loop),
		_status(kConnecting),
		_socket(socket)
	{
		_socket->setReadCallback(std::bind(&Connection::handleReadInLoop, this));
		_socket->setWriteCallback(std::bind(&Connection::handleWriteInLoop, this));
		_socket->setCloseCallback(std::bind(&Connection::handleCloseInLoop, this));
		_socket->setErrorCallback(std::bind(&Connection::handleErrorInLoop, this));
	}

	Connection::~Connection()
	{
		assert(_status == kDisconnected);
	}

	void Connection::forceClose()
	{
		if (_status != kConnected)
			return;

		changeStatus(kDisconnecting);

		auto self = shared_from_this();
		_loop->queueInLoop([self]()
		{
			self->handleCloseInLoop();
		});
	}

	void Connection::shutdown()
	{
		if (_status != kConnected)
			return;

		changeStatus(kDisconnecting);

		auto self = shared_from_this();
		_loop->queueInLoop([self]()
		{
			if (!self->_socket->isWriting())
				self->_socket->shutdown();
		});
	}

	void Connection::send(const NetString &data)
	{
		if (_status != kConnected)
			return;

		if (_loop->isInLoopThread())
		{
			sendInLoop(data.getData(), data.getLength());
		}
		else
		{
			std::shared_ptr<std::string> copied(new std::string(data.toString()));
			_loop->queueInLoop([this, copied]()
			{
				sendInLoop(copied->c_str(), copied->size());
			});
		}
	}

	void Connection::send(const char *data, size_t len)
	{
		this->send(NetString(data, len));
	}

	void Connection::sendInLoop(const void *data, size_t len)
	{
		_loop->assertInLoopThread();

		_socket->enableWriteInLoop();
		_outputBuffer.append(data, len);
	}

	void Connection::changeStatus(int status)
	{
		_status = status;
	}

	void Connection::handleReadInLoop()
	{
		_loop->assertInLoopThread();

		ssize_t n = _inputBuffer.readFd(_socket->getFd());
		if (n > 0)
		{
			if (_observer)
				_observer->onReceivedMessageInConnection(shared_from_this(), &_inputBuffer);
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

	void Connection::handleWriteInLoop()
	{
		_loop->assertInLoopThread();

		if (_socket->isWriting())
		{
			ssize_t n = ::write(_socket->getFd(),
								_outputBuffer.peek(),
								_outputBuffer.readableBytes());
			if (n > 0)
			{
				_outputBuffer.retrieve(static_cast<size_t >(n));
				if (_outputBuffer.readableBytes() == 0)
				{
					_socket->disableWriteInLoop();
					if (_observer)
						_observer->onWriteCompletedInConnection(shared_from_this());
				}
			}
			else
			{
				LOG_ERROR("TcpConnection::handleWriteInLoop")
			}
		}
		else
		{
			LOG_DEBUG("Connection(%d) is down, no more writing.", getIndex());
		}
	}

	void Connection::handleCloseInLoop()
	{
		_loop->assertInLoopThread();

		LOG_DEBUG("Connection(%d) state = %s", getIndex(), gStatusNames[_status]);

		assert(_status == kConnected || _status == kDisconnecting);

		_socket->detachInLoop();
		changeStatus(kDisconnected);

		if (_observer)
		{
			ConnectionPtr guardThis(shared_from_this());
			_observer->onConnectChangedInConnection(guardThis);
			_observer->onClosedInConnection(guardThis);
			_observer = nullptr;
		}
	}

	void Connection::handleErrorInLoop()
	{
		_loop->assertInLoopThread();

		int err = _socket->getSocketError();
		LOG_ERROR("TcpConnection::handleErrorInLoop [%d] - SO_ERROR(%d): %s", getIndex(), err, errnoToString(err));
	}

	void Connection::onConnectEstablishedInLoop()
	{
		_loop->assertInLoopThread();

		changeStatus(kConnected);
		_socket->attachInLoop(_loop);
		_socket->enableReadInLoop();

		if (_observer)
			_observer->onConnectChangedInConnection(shared_from_this());
	}

	void Connection::onConnectDestroyedInLoop()
	{
		_loop->assertInLoopThread();

		/// redundant code
		_socket->detachInLoop();
		changeStatus(kDisconnected);
	}
}