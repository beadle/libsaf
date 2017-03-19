//
// Created by beadle on 3/19/17.
//

#ifndef EXAMPLE_TCPSERVER_H
#define EXAMPLE_TCPSERVER_H

#include <memory>
#include <string>
#include <unordered_map>

#include "net/InetAddress.h"


namespace saf
{

	class EventLoop;
	class Acceptor;
	class TcpConnection;

	class TcpServer
	{
	public:
		TcpServer(EventLoop* loop, const InetAddress& addr);
		~TcpServer();

		void start();

	protected:
		void handleAccept(int connfd, const InetAddress& addr);

	private:
		typedef std::unique_ptr<TcpConnection> ConnPtr;
		typedef std::unordered_map<int, ConnPtr > ConnMap;

		bool _started;
		int _connCount;
		EventLoop* _loop;
		std::unique_ptr<Acceptor> _acceptor;
		ConnMap _connMap;
	};

}

#endif //EXAMPLE_TCPSERVER_H
