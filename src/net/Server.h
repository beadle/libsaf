//
// Created by beadle on 3/27/17.
//

#ifndef EXAMPLE_SERVER_H
#define EXAMPLE_SERVER_H

#include "Types.h"
#include "InetAddress.h"


namespace saf
{
	class Acceptor;
	class EventLoop;
	class EventLoopCluster;

	class Server : public std::enable_shared_from_this<Server>
	{
	public:
		Server(EventLoop* loop);
		virtual ~Server();

	public:  /// Thread-Safed Methods
		virtual void start(const InetAddress& addr, size_t threadCount) = 0;
		virtual void stop() = 0;

		EventLoop* getLoop() const { return _loop; }

		/// Set connection callback.
		/// Called from connection's thread
		void setConnectChangeCallback(ConnectChangeCallback&& cb)
		{ _connectChangeCallback = std::move(cb); }

		/// Set message callback.
		/// Called from connection's thread
		void setRecvMessageCallback(RecvMessageCallback&& cb)
		{ _recvMessageCallback = std::move(cb); }

		/// Set write complete callback.
		/// Called from connection's thread
		void setWriteCompleteCallback(WriteCompleteCallback&& cb)
		{ _writeCompleteCallback = std::move(cb); }

	protected:
		virtual void bindDefaultCallbacks(Connection* conn);
		virtual void unbindDefaultCallbacks(Connection* conn);
		virtual void onClosedInConnection(const ConnectionPtr&) = 0;

		void notifyConnectionEstablished(const ConnectionPtr&);

	protected:
		bool _running;
		EventLoop* _loop;

		std::unique_ptr<EventLoopCluster> _cluster;

		RecvMessageCallback _recvMessageCallback;
		WriteCompleteCallback _writeCompleteCallback;
		ConnectChangeCallback _connectChangeCallback;
	};

}

#endif //EXAMPLE_SERVER_H
