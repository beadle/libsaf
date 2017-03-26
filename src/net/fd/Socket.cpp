//
// Created by beadle on 3/14/17.
//
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include "Socket.h"
#include "net/EventLoop.h"
#include "base/Logging.h"

namespace saf
{

	Socket* Socket::create(NetProtocal protocal, sa_family_t family)
	{
		auto flags = SOCK_NONBLOCK | SOCK_CLOEXEC;
		auto proto = 0;

		switch (protocal)
		{
			case NetProtocal::TCP:
				flags |= SOCK_STREAM;
				proto = IPPROTO_TCP;
				break;
			default:
				flags |= SOCK_DGRAM;
				break;
		}

		int sockfd = ::socket(family, flags, proto);
		if (sockfd < 0)
		{
			LOG_FATAL("Socket::create");
		}
		return new Socket(sockfd);
	}

	Socket::Socket(int fd):
		IOFd(fd)
	{

	}

	Socket::~Socket()
	{
		::close(getFd());
	}

	void Socket::bind(const InetAddress &localAddr)
	{
		int ret = ::bind(getFd(), localAddr.getSockAddr(), static_cast<socklen_t>(sizeof(struct sockaddr_in6)));
		if (ret < 0)
		{
			LOG_FATAL("Socket::bind")
		}
	}

	void Socket::listen()
	{
		int ret = ::listen(getFd(), SOMAXCONN);
		if (ret < 0)
		{
			LOG_FATAL("Socket::listenInLoop")
		}
	}

	int Socket::accept(InetAddress &peerAddr)
	{
		auto addr = peerAddr.getSockAddr();
		socklen_t addrlen = static_cast<socklen_t>(sizeof *addr);
		int connfd = ::accept4(getFd(), (sockaddr*)addr, &addrlen, SOCK_NONBLOCK | SOCK_CLOEXEC);
		if (connfd < 0)
		{
			LOG_FATAL("Socket::accept")
		}
		return connfd;
	}

	int Socket::connect(const InetAddress& serverAddr)
	{
		int ret = ::connect(getFd(), serverAddr.getSockAddr(), static_cast<socklen_t>(sizeof(struct sockaddr_in)));
		return ret;
	}

	void Socket::shutdown()
	{
		int ret = ::shutdown(getFd(), SHUT_WR);
		if (ret < 0)
		{
			LOG_FATAL("Socket::shutdown")
		}
	}

	int Socket::getSocketError()
	{
		int optval;
		socklen_t optlen = static_cast<socklen_t>(sizeof optval);

		if (::getsockopt(getFd(), SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0)
		{
			return errno;
		}
		else
		{
			return optval;
		}
	}

	void Socket::setKeepAlive(bool on)
	{
		int optval = on ? 1 : 0;
		::setsockopt(getFd(), SOL_SOCKET, SO_KEEPALIVE,
					 &optval, static_cast<socklen_t>(sizeof optval));
	}

	void Socket::setTcpNoDelay(bool on)
	{
		int optval = on ? 1 : 0;
		::setsockopt(getFd(), IPPROTO_TCP, TCP_NODELAY,
					 &optval, static_cast<socklen_t>(sizeof optval));
	}

	void Socket::setReuseAddr(bool on)
	{
		int optval = on ? 1 : 0;
		::setsockopt(getFd(), SOL_SOCKET, SO_REUSEADDR,
					 &optval, static_cast<socklen_t>(sizeof optval));
	}

	void Socket::setReusePort(bool on)
	{
		int optval = on ? 1 : 0;
		int ret = ::setsockopt(getFd(), SOL_SOCKET, SO_REUSEPORT,
					 &optval, static_cast<socklen_t>(sizeof optval));
		if (ret < 0 && on)
		{
			LOG_ERROR("SO_REUSEPORT is not supported.");
		}
	}

}