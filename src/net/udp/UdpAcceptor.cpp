//
// Created by beadle on 3/27/17.
//

#include "UdpAcceptor.h"
#include "net/EventLoop.h"
#include "net/fd/Socket.h"
#include "base/Logging.h"

namespace saf
{

	UdpAcceptor::UdpAcceptor(EventLoop *loop) :
		Acceptor(loop)
	{

	}

	UdpAcceptor::~UdpAcceptor()
	{

	}

	void UdpAcceptor::listenInLoop(const InetAddress& addr, bool reusePort)
	{
		_loop->assertInLoopThread();

		if (_listening)
			return;
		_listening = true;

		_addr = addr;
		_socket.reset(Socket::create(NetProtocal::UDP, _addr.getFamily()));

		_socket->attachInLoop(_loop);
		_socket->setReuseAddr(true);
		_socket->setReusePort(reusePort);
		_socket->setObserver(this);
//		_socket->setReadCallback(std::bind(&UdpAcceptor::handleReadInLoop, this));
//		_socket->setWriteCallback(std::bind(&UdpAcceptor::handleWriteInLoop, this));
//		_socket->setErrorCallback(std::bind(&UdpAcceptor::handleErrorInLoop, this));
//		_socket->setCloseCallback(std::bind(&UdpAcceptor::handleCloseInLoop, this));
		_socket->bind(_addr);
		_socket->enableReadInLoop();
	}

	void UdpAcceptor::stopInLoop()
	{
		_loop->assertInLoopThread();

		_listening = false;

		if (_socket)
		{
			_socket->detachInLoop();
			_socket.reset();
		}
	}

	void UdpAcceptor::handleReadInLoop()
	{
		_loop->assertInLoopThread();

		InetAddress peerAddr;
		_inputBuffer.readFrom(_socket->getFd(), peerAddr);
		if (_recvMessageCallback)
			_recvMessageCallback(peerAddr, &_inputBuffer);
		_inputBuffer.retrieveAll();
	}

	void UdpAcceptor::handleWriteInLoop()
	{
		_loop->assertInLoopThread();
	}

	void UdpAcceptor::handleCloseInLoop()
	{
		_loop->assertInLoopThread();
	}

	void UdpAcceptor::handleErrorInLoop()
	{
		_loop->assertInLoopThread();

		int err = _socket->getSocketError();
		LOG_ERROR("TcpConnection::handleErrorInLoop - SO_ERROR(%d): %s", err, errnoToString(err));
	}

	void UdpAcceptor::onReadInIOFd(IOFd*)
	{
		handleReadInLoop();
	}

	void UdpAcceptor::onWriteInIOFd(IOFd*)
	{
		handleWriteInLoop();
	}

	void UdpAcceptor::onErrorInIOFd(IOFd*)
	{
		handleErrorInLoop();
	}

	void UdpAcceptor::onCloseInIOFd(IOFd*)
	{
		handleCloseInLoop();
	}

}