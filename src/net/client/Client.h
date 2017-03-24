//
// Created by beadle on 3/24/17.
//

#ifndef EXAMPLE_CLIENT_H
#define EXAMPLE_CLIENT_H

#include <memory>
#include <atomic>

#include "net/InetAddress.h"
#include "net/Types.h"


namespace saf
{
	class Connector;
	class EventLoop;
	class Connection;
	class Socket;

	class Client : public ConnectionObserver
	{
	public:
		Client(const InetAddress& addr, NetProtocal protocal, float reconnectDelay);
		~Client();

		void connect();
		void disconnect();

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
		bool onConnectedCallback(std::unique_ptr<Socket>& socket);

		void onConnReceivedMessage(const ConnectionPtr&, Buffer*);
		void onConnWriteCompleted(const ConnectionPtr&);
		void onConnConnectChanged(const ConnectionPtr&);
		void onConnClosed(const ConnectionPtr&);

		void removeConnection(const ConnectionPtr& conn);

	private:
		std::atomic_bool _connecting;
		InetAddress _addr;
		NetProtocal _protocal;
		float _reconnectDelay;

		std::unique_ptr<EventLoop> _loop;
		std::unique_ptr<Connector> _connector;
		std::shared_ptr<Connection> _connection;

		RecvMessageCallback _recvMessageCallback;
		WriteCompleteCallback _writeCompleteCallback;
		ConnectChangeCallback _connectChangeCallback;

	};

}

#endif //EXAMPLE_CLIENT_H
