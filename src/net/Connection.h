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

		virtual void close() = 0;

		virtual bool isConnected() const = 0;
		virtual void setTcpNoDelay(bool on) = 0;

		void send(NetString data);
		void send(const char* data, size_t length);

		inline long getActivedTime() const { return _activedTime; }
		inline EventLoop* getLooper() const { return _loop; }
		inline const std::string& getIndex() const { return _index; }
		inline const InetAddress& getAddress() const { return _addr; }

		/// Set connection callback.
		/// Called from connection's thread
		void setConnectChangeCallback(ConnectChangeCallback&& cb)
		{ _connectChangeCallback = std::move(cb); }
		void setConnectChangeCallback(const ConnectChangeCallback& cb)
		{ _connectChangeCallback = cb; }

		/// Set message callback.
		/// Called from connection's thread
		void setRecvMessageCallback(RecvMessageCallback&& cb)
		{ _recvMessageCallback = std::move(cb); }
		void setRecvMessageCallback(const RecvMessageCallback& cb)
		{ _recvMessageCallback = cb; }

		/// Set write complete callback.
		/// Called from connection's thread
		void setWriteCompleteCallback(WriteCompleteCallback&& cb)
		{ _writeCompleteCallback = std::move(cb); }
		void setWriteCompleteCallback(const WriteCompleteCallback& cb)
		{ _writeCompleteCallback = cb; }

	protected:  /// Friend Methods
		Connection(EventLoop* loop,
				   const std::string& key,
				   const InetAddress& addr);

		/// Set write complete callback.
		/// Called from connection's thread
		void setCloseCallback(CloseCallback&& cb)
		{ _closeCallback = std::move(cb); }

	protected:  /// Looper Thread Methods
		virtual void sendInLoop(const char* data, size_t length) = 0;
		virtual void onConnectEstablishedInLoop() = 0;

		friend class Server;
		friend class Client;

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
