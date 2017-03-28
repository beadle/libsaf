//
// Created by beadle on 3/27/17.
//

#ifndef EXAMPLE_UDPSERVER_H
#define EXAMPLE_UDPSERVER_H

#include <string>
#include <unordered_map>
#include <memory>

#include "net/Server.h"

namespace saf
{
	class Buffer;
	class UdpAcceptor;
	class UdpConnection;
	class EventLoop;
	class EventLoopCluster;

	class UdpServer : public Server
	{
	public: /// Thread-Safed Methods
		UdpServer(EventLoop* loop);
		~UdpServer();

		void start(const InetAddress& addr, size_t threadCount);
		void stop();

	protected: /// Looper Thread Methods
		typedef std::shared_ptr<UdpConnection> UdpConnectionPtr;
		typedef std::unordered_map<std::string, UdpConnectionPtr> ConnectionMap;

		void recvMessageInLoop(InetAddress& addr, Buffer* buffer);
		void checkExpiredInLoop();

	protected:  /// Connection Thread Methods
		void onClosedInConnection(const ConnectionPtr&);

	private:
		void bindDefaultCallbacks(Connection* conn);
		void unbindDefaultCallbacks(Connection* conn);

	private:
		int _expiredChecker;
		std::unique_ptr<UdpAcceptor> _acceptor;
		ConnectionMap _connections;
	};

}

#endif //EXAMPLE_UDPSERVER_H
