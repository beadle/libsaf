//
// Created by beadle on 3/19/17.
//

#ifndef EXAMPLE_TCP_SERVER_H
#define EXAMPLE_TCP_SERVER_H

#include <memory>
#include <string>
#include <unordered_map>

#include "net/Server.h"


namespace saf
{
	class TcpAcceptor;
	class TcpConnection;

	class TcpServer : public Server
	{
	public:
		TcpServer(EventLoop* loop);
		~TcpServer();

	public:  /// Thread-Safed Methods
		void start(const InetAddress& addr, size_t threadCount);
		void stop();

	protected:  /// Looper Thread Methods
		typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
		typedef std::unordered_map<std::string, TcpConnectionPtr> ConnectionMap;

		void newConnectionInLoop(int connfd, const InetAddress &addr);
		void removeConnectionInLoop(const ConnectionPtr &conn);

		TcpConnectionPtr createConnectionInLoop(int connfd, const InetAddress& addr);

	protected:  /// Connection Thread Methods
		void onClosedInConnection(const ConnectionPtr&);

	private:
		std::unique_ptr<TcpAcceptor> _acceptor;
		ConnectionMap _connections;
	};

}

#endif //EXAMPLE_SERVER_H
