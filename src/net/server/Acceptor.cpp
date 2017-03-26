//
// Created by beadle on 3/14/17.
//

#include <unistd.h>
#include <fcntl.h>

#include "Acceptor.h"
#include "net/fd/Socket.h"
#include "net/EventLoop.h"
#include "base/Logging.h"


namespace saf
{
	Acceptor::Acceptor(EventLoop *loop, const InetAddress &addr, NetProtocal protocal, bool reusePort):
		_listening(false),
		_reusePort(reusePort),
		_loop(loop),
		_addr(addr),
		_protocal(protocal),
		_socket(nullptr),
		_idleFd(::open("/dev/null", O_RDONLY | O_CLOEXEC))
	{

	}

	Acceptor::~Acceptor()
	{
		stopInLoop();
		::close(_idleFd);
	}

	void Acceptor::listenInLoop()
	{
		_loop->assertInLoopThread();

		if (_listening)
			return;
		_listening = true;

		_socket.reset(Socket::create(_protocal, _addr.getFamily()));
		_socket->attachInLoop(_loop);
		_socket->setReuseAddr(true);
		_socket->setReusePort(_reusePort);
		_socket->setReadCallback(std::bind(&Acceptor::handleReadInLoop, this));
		_socket->bind(_addr);
		_socket->listen();
		_socket->enableReadInLoop();
	}

	void Acceptor::stopInLoop()
	{
		_loop->assertInLoopThread();

		_listening = false;

		if (_socket)
		{
			_socket->detachInLoop();
			_socket.reset();
		}
	}

	void Acceptor::handleReadInLoop()
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
			LOG_ERROR("Acceptor::handleReadInLoop")
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