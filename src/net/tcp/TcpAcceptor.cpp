//
// Created by beadle on 3/14/17.
//

#include <unistd.h>
#include <fcntl.h>

#include "TcpAcceptor.h"
#include "net/fd/Socket.h"
#include "net/EventLoop.h"
#include "base/Logging.h"


namespace saf
{
	TcpAcceptor::TcpAcceptor(EventLoop *loop):
		Acceptor(loop),
		_idleFd(::open("/dev/null", O_RDONLY | O_CLOEXEC))
	{

	}

	TcpAcceptor::~TcpAcceptor()
	{
		stopInLoop();
		::close(_idleFd);
	}

	void TcpAcceptor::listenInLoop(const InetAddress& addr, bool reusePort)
	{
		_loop->assertInLoopThread();

		if (_listening)
			return;
		_listening = true;

		_addr = addr;
		_socket.reset(Socket::create(NetProtocal::TCP, _addr.getFamily()));

		_socket->attachInLoop(_loop);
		_socket->setReuseAddr(true);
		_socket->setReusePort(reusePort);
		_socket->setReadCallback(std::bind(&TcpAcceptor::handleReadInLoop, this));
		_socket->bind(_addr);
		_socket->enableReadInLoop();
		_socket->listen();
	}

	void TcpAcceptor::stopInLoop()
	{
		_loop->assertInLoopThread();

		_listening = false;

		if (_socket)
		{
			_socket->detachInLoop();
			_socket.reset();
		}
	}

	void TcpAcceptor::handleReadInLoop()
	{
		_loop->assertInLoopThread();

		InetAddress peerAddr(0);
		int connfd = _socket->accept(peerAddr);
		if (connfd >= 0)
		{
			if (_callback)
				_callback(connfd, peerAddr);
			else
				::close(connfd);
		}
		else
		{
			LOG_ERROR("TcpAcceptor::handleReadInLoop")
			// Read the section named "The special problem of
			// accept()ing when you can't" in libev's doc.
			// By Marc Lehmann, author of libev.
			if (errno == EMFILE)
			{
				::close(_idleFd);
				_idleFd = ::accept(_socket->getFd(), NULL, NULL);
				::close(_idleFd);
				_idleFd = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
			}
		}
	}
}