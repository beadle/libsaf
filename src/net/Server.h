//
// Created by beadle on 3/19/17.
//

#ifndef EXAMPLE_SERVER_H
#define EXAMPLE_SERVER_H

#include <memory>
#include <string>
#include <unordered_map>

#include "net/InetAddress.h"
#include "net/Types.h"


namespace saf
{
	class EventLoop;
	class EventLoopCluster;
	class Acceptor;

	class Server : public ConnectionObserver
	{
	public:
		Server(const InetAddress& addr, NetProtocal protocal);
		~Server();

		void start(size_t threadCount);
		void stop();

		EventLoop* getLoop() const { return _loop.get(); }

		/// Set connection callback.
		/// Not thread safe.
		/// Called from connection's thread
		void setConnectChangeCallback(const ConnectChangeCallback& cb)
		{ _connectChangeCallback = std::move(cb); }

		/// Set message callback.
		/// Not thread safe.
		/// Called from connection's thread
		void setRecvMessageCallback(const RecvMessageCallback& cb)
		{ _recvMessageCallback = std::move(cb); }

		/// Set write complete callback.
		/// Not thread safe.
		/// Called from connection's thread
		void setWriteCompleteCallback(const WriteCompleteCallback& cb)
		{ _writeCompleteCallback = std::move(cb); }

	protected:
		void newConnection(int connfd, const InetAddress &addr);
		void removeConnection(const ConnectionPtr& conn);

		/// ConnectionObserver's interfaces
		/// Called by TcpConnection from other EventLoop(Thread)s
		void onConnReceivedMessage(const ConnectionPtr&, Buffer*);
		void onConnWriteCompleted(const ConnectionPtr&);
		void onConnConnectChanged(const ConnectionPtr&);
		void onConnClosed(const ConnectionPtr&);

		ConnectionPtr createConnection(int connfd);

	private:
		typedef std::unordered_map<int, std::shared_ptr<Connection> > ConnectionMap;

		bool _started;
		int _connCount;
		NetProtocal _protocal;

		std::unique_ptr<EventLoop> _loop;
		std::unique_ptr<Acceptor> _acceptor;
		std::unique_ptr<EventLoopCluster> _cluster;

		RecvMessageCallback _recvMessageCallback;
		WriteCompleteCallback _writeCompleteCallback;
		ConnectChangeCallback _connectChangeCallback;

		ConnectionMap _connections;
	};

}

#endif //EXAMPLE_SERVER_H
