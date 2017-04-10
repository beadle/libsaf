//
// Created by beadle on 3/28/17.
//

#ifndef EXAMPLE_CLIENT_H
#define EXAMPLE_CLIENT_H

#include <atomic>

#include "InetAddress.h"
#include "Types.h"


namespace saf
{
	class EventLoop;

	class Client : public std::enable_shared_from_this<Client>
	{
	public:  /// Thread-Safed Methods
		Client(EventLoop* loop);
		virtual ~Client() {}

		virtual void connect(const InetAddress&) = 0;
		virtual void disconnect() = 0;

		/// Set connection callback.
		/// Called from connection's thread
		void setConnectChangeCallback(const ConnectChangeCallback& cb)
		{ _connectChangeCallback = std::move(cb); }

		/// Set message callback.
		/// Called from connection's thread
		void setRecvMessageCallback(const RecvMessageCallback& cb)
		{ _recvMessageCallback = std::move(cb); }

		/// Set write complete callback.
		/// Called from connection's thread
		void setWriteCompleteCallback(const WriteCompleteCallback& cb)
		{ _writeCompleteCallback = std::move(cb); }

		void setReconnectDelay(float delay) { _reconnectDelay = delay; }
		float getReconnectDelay() const { return _reconnectDelay; }

		virtual ConnectionPtr getConnection() const = 0;

	protected:
		virtual void onClosedInConnection(const ConnectionPtr& conn) = 0;
		virtual void bindDefaultCallbacks(Connection* ptr);
		virtual void unbindDefaultCallbacks(Connection* ptr);

		void notifyConnectionEstablished(const ConnectionPtr&);

	protected:
		float _reconnectDelay;
		EventLoop* _loop;
		InetAddress _addr;
		std::atomic_bool _connecting;

		RecvMessageCallback _recvMessageCallback;
		WriteCompleteCallback _writeCompleteCallback;
		ConnectChangeCallback _connectChangeCallback;
	};

}

#endif //EXAMPLE_CLIENT_H
