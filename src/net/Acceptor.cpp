//
// Created by beadle on 3/14/17.
//

#include <unistd.h>
#include <fcntl.h>

#include "Acceptor.h"
#include "fd/Socket.h"
#include "EventLoop.h"
#include "base/Logging.h"


namespace saf
{
	Acceptor::Acceptor(EventLoop *loop, const InetAddress &addr, NetProtocal protocal, bool reusePort):
		_listening(false),
		_loop(loop),
		_addr(addr),
		_socket(Socket::create(loop, protocal, addr.getFamily())),
		_idleFd(::open("/dev/null", O_RDONLY | O_CLOEXEC))
	{
		_socket->setReuseAddr(true);
		_socket->setReusePort(reusePort);
		_socket->setReadCallback(std::bind(&Acceptor::handleRead, this));
		_socket->bind(addr);
	}

	Acceptor::~Acceptor()
	{
		_socket->disableAll();
		_loop->removeFd(_socket.get());
		::close(_idleFd);
	}

	void Acceptor::listen()
	{
		_loop->assertInLoopThread();
		_listening = true;
		_socket->listen();
		_socket->enableRead();
	}

	void Acceptor::handleRead()
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
			LOG_ERROR("Acceptor::handleRead")
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