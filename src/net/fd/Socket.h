//
// Created by beadle on 3/14/17.
//

#ifndef LIBSAF_SOCKET_H
#define LIBSAF_SOCKET_H


#include "IOFd.h"
#include "net/InetAddress.h"
#include "net/Types.h"


namespace saf
{
	class Socket : public IOFd
	{
	public:
		static Socket* create(
				EventLoop* loop, NetProtocal protocal=NetProtocal::TCP, sa_family_t family=AF_INET);

	public:
		Socket(EventLoop* loop, int fd);
		~Socket();

		void bind(const InetAddress& localAddr);
		void listen();
		int accept(InetAddress& peerAddr);
		int connect(const InetAddress& serverAddr);

		void shutdown();

		int getSocketError();

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
