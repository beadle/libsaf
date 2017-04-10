//
// Created by beadle on 3/27/17.
//

#include "UdpConnection.h"
#include "base/Logging.h"
#include "base/TimeUtils.h"
#include "net/EventLoop.h"
#include "net/Acceptor.h"
#include "net/fd/Socket.h"


namespace saf
{

	UdpConnection::UdpConnection(
			EventLoop *loop, const std::string& key, const InetAddress &addr) :
		Connection(loop, key, addr),
		_writing(false)
	{

	}

	UdpConnection::~UdpConnection()
	{

	}

	void UdpConnection::close()
	{
		auto pin = shared_from_this();
		_loop->runInLoop([this, pin]()
		{
			closeInLoop();
		});
	}

	void UdpConnection::handleReadInLoop(const char *data, size_t length)
	{
		_loop->assertInLoopThread();

		_inputBuffer.append(data, length);
		_activedTime = time::timestamp();

		if (_recvMessageCallback)
			_recvMessageCallback(shared_from_this(), &_inputBuffer);
		else
			_inputBuffer.retrieveAll();
	}

	void UdpConnection::closeInLoop()
	{
		onConnectDestroyedInLoop();

		if (_closeCallback)
			_closeCallback(shared_from_this());
	}

	void UdpConnection::sendInLoop(const char *data, size_t length)
	{
		_loop->assertInLoopThread();

		_outputBuffer.append(data, length);

		if (!_writing)
		{
			_writing = true;
			handleWriteInLoop();
		}
	}

	void UdpConnection::handleWriteInLoop()
	{
		_loop->assertInLoopThread();

		if (!_senderFd)
			return;

		ssize_t n = ::sendto(
				_senderFd,
				_outputBuffer.peek(),
				_outputBuffer.readableBytes(),
				MSG_DONTWAIT,
				_addr.getSockAddrRaw(),
				static_cast<socklen_t>(sizeof(struct sockaddr_in)));

		if (n > 0)
		{
			_outputBuffer.retrieve(static_cast<size_t >(n));
			if (_outputBuffer.readableBytes() == 0)
			{
				_writing = false;
				if (_writeCompleteCallback)
					_writeCompleteCallback(shared_from_this());
			}
			else
			{
				_loop->queueInLoop([this]()
				{
					handleWriteInLoop();
				});
			}
		}
		else
		{
			LOG_ERROR("TcpConnection::handleWriteInLoop");
		}
	}

	void UdpConnection::onConnectEstablishedInLoop()
	{
		_loop->assertInLoopThread();

		_connecting = true;

		if (_connectChangeCallback)
			_connectChangeCallback(shared_from_this());
	}

	void UdpConnection::onConnectDestroyedInLoop()
	{
		_loop->assertInLoopThread();

		_connecting = false;

		if (_connectChangeCallback)
			_connectChangeCallback(shared_from_this());
	}

}