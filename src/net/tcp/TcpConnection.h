//
// Created by beadle on 3/23/17.
//

#ifndef EXAMPLE_TCP_CONNECTION_H
#define EXAMPLE_TCP_CONNECTION_H

#include <atomic>
#include <memory>

#include "net/Buffer.h"
#include "net/Connection.h"


namespace saf
{
	class Socket;
	class EventLoop;

	class TcpConnection : public Connection
	{
	public:
		enum {
			kDisconnected,
			kConnecting,
			kConnected,
			kDisconnecting,
		};

	public: /// Thread-Safed Methods
		TcpConnection(EventLoop* loop,
					  Socket* socket,
					  const std::string& index,
					  const InetAddress& addr);
		~TcpConnection();

		bool isConnected() const { return _status == kConnected; }
		bool isDisconnected() const { return _status == kDisconnected; }

		void shutdown();
		void forceClose();

	protected:  /// Friend Methods
		void changeStatus(int status);

		friend class TcpServer;
		friend class TcpClient;

	protected:  /// Looper Thread Methods
		void sendInLoop(const char* data, size_t len);
		ssize_t readInLoop();
		ssize_t writeInLoop(const char* buffer, size_t length);

		void handleReadInLoop();
		void handleWriteInLoop();
		void handleErrorInLoop();
		void handleCloseInLoop();

		void onConnectEstablishedInLoop();
		void onConnectDestroyedInLoop();

	protected:
		std::unique_ptr<Socket> _socket;
		std::atomic_int _status;

		Buffer _inputBuffer;
		Buffer _outputBuffer;
	};


}

#endif //EXAMPLE_CONNECTION_H
