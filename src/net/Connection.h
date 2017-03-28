//
// Created by beadle on 3/27/17.
//

#ifndef EXAMPLE_CONNECTION_H
#define EXAMPLE_CONNECTION_H

#include <memory>
#include <string>

#include "NetString.h"
#include "InetAddress.h"
#include "Types.h"


namespace saf
{
	class EventLoop;

	class Connection : public std::enable_shared_from_this<Connection>
	{
	public: /// Thread-Safed Methods
		virtual ~Connection();

		void send(NetString data);
		void send(const char* data, size_t length);

		long getActivedTime() const { return _activedTime; }
		EventLoop* getLooper() const { return _loop; }
		const std::string& getIndex() const { return _index; }
		const InetAddress& getAddress() const { return _addr; }

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

	protected:  /// Friend Methods
		Connection(EventLoop* loop,
				   const std::string& key,
				   const InetAddress& addr);

		/// Set write complete callback.
		/// Called from connection's thread
		void setCloseCallback(CloseCallback&& cb)
		{ _closeCallback = std::move(cb); }

		// FIXME: ugly
		friend class Server;
		friend class Client;
		friend class TcpServer;
		friend class UdpServer;
		friend class TcpClient;

	protected:  /// Looper Thread Methods
		virtual void sendInLoop(const char* data, size_t length) = 0;

		virtual void onConnectEstablishedInLoop() = 0;
		virtual void onConnectDestroyedInLoop() = 0;

	protected:
		long _activedTime;
		EventLoop* _loop;

		InetAddress _addr;
		std::string _index;

		RecvMessageCallback _recvMessageCallback;
		WriteCompleteCallback _writeCompleteCallback;
		ConnectChangeCallback _connectChangeCallback;
		CloseCallback _closeCallback;
	};

}

#endif //EXAMPLE_CONNECTION_H
