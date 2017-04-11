//
// Created by beadle on 3/14/17.
//

#ifndef LIBSAF_SOCKET_H
#define LIBSAF_SOCKET_H


#include <base/Noncopyable.h>
#include "IOFd.h"
#include "net/InetAddress.h"
#include "net/Types.h"


namespace saf
{
	class Socket : public IOFd, public Noncopyable
	{
	public:
		static Socket* create(NetProtocal protocal, sa_family_t family=AF_INET);

	public:
		Socket(int fd, NetProtocal protocal);
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

	private:
		NetProtocal _protocal;

	};
}


#endif //LIBSAF_SOCKET_H
