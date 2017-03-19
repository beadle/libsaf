//
// Created by beadle on 3/14/17.
//

#ifndef LIBSAF_SOCKET_H
#define LIBSAF_SOCKET_H

#include <sys/socket.h>

#include "IOFd.h"
#include "net/InetAddress.h"


namespace saf
{
	class Socket : public IOFd
	{
	public:
		static Socket* create(EventLoop* loop, sa_family_t family=AF_INET);

	public:
		Socket(EventLoop* loop, int fd);
		~Socket();

		void bind(const InetAddress& localAddr);
		void listen();
		int accept(InetAddress& peerAddr);

		/// Enable/disable TCP_NODELAY (disable/enable Nagle's algorithm).
		void setTcpNoDelay(bool on);
		/// Enable/disable SO_REUSEADDR
		void setReuseAddr(bool on);
		/// Enable/disable SO_REUSEPORT
		void setReusePort(bool on);
		/// Enable/disable SO_KEEPALIVE
		void setKeepAlive(bool on);

	};
}


#endif //LIBSAF_SOCKET_H
