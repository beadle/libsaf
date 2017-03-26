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
		Server(EventLoop* loop, const InetAddress& addr, NetProtocal protocal);
		~Server();

	public:  /// Thread-Safed Methods
		void start(size_t threadCount);
		void stop();

		EventLoop* getLoop() const { return _loop; }

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

	protected:  /// Looper Thread Methods
		void newConnectionInLoop(int connfd, const InetAddress &addr);
		void removeConnectionInLoop(const ConnectionPtr &conn);
		ConnectionPtr createConnectionInLoop(int connfd);

	protected:  /// Connection Thread Methods
		void onReceivedMessageInConnection(const ConnectionPtr&, Buffer*);
		void onWriteCompletedInConnection(const ConnectionPtr&);
		void onConnectChangedInConnection(const ConnectionPtr&);
		void onClosedInConnection(const ConnectionPtr&);

	private:
		typedef std::unordered_map<int, std::shared_ptr<Connection> > ConnectionMap;

		bool _running;
		NetProtocal _protocal;
		EventLoop* _loop;

		std::unique_ptr<Acceptor> _acceptor;
		std::unique_ptr<EventLoopCluster> _cluster;

		RecvMessageCallback _recvMessageCallback;
		WriteCompleteCallback _writeCompleteCallback;
		ConnectChangeCallback _connectChangeCallback;

		ConnectionMap _connections;
	};

}

#endif //EXAMPLE_SERVER_H
