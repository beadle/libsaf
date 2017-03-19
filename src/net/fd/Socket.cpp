//
// Created by beadle on 3/14/17.
//
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include "Socket.h"
#include "../EventLoop.h"

namespace saf
{

	Socket* Socket::create(EventLoop* loop, sa_family_t family)
	{
		int sockfd = ::socket(family, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
		if (sockfd < 0)
		{
			// TODO: log error
		}
		return new Socket(loop, sockfd);
	}

	Socket::Socket(EventLoop *loop, int fd):
		IOFd(loop, fd)
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
			// TODO: log error
		}
	}

	void Socket::listen()
	{
		int ret = ::listen(getFd(), SOMAXCONN);
		if (ret < 0)
		{
			// TODO: log error
		}
	}

	int Socket::accept(InetAddress &peerAddr)
	{
		auto addr = peerAddr.getSockAddr();
		socklen_t addrlen = static_cast<socklen_t>(sizeof *addr);
		int connfd = ::accept4(getFd(), (sockaddr*)addr, &addrlen, SOCK_NONBLOCK | SOCK_CLOEXEC);
		if (connfd < 0)
		{
			// todo: log error
		}
		return connfd;
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
		::setsockopt(getFd(), SOL_SOCKET, SO_REUSEPORT,
					 &optval, static_cast<socklen_t>(sizeof optval));
	}

}