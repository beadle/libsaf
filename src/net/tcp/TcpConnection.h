//
// Created by beadle on 3/23/17.
//

#ifndef EXAMPLE_CONNECTION_H
#define EXAMPLE_CONNECTION_H

#include <memory>
#include <atomic>
#include "net/Buffer.h"
#include "net/Types.h"


namespace saf
{

	class Socket;
	class Buffer;
	class EventLoop;

	class TcpConnection : public std::enable_shared_from_this<TcpConnection>
	{
	public:
		enum {
			kDisconnected,
			kConnecting,
			kConnected,
			kDisconnecting,
		};

	public: /// Thread-Safed Methods
		TcpConnection(EventLoop* loop, Socket* socket, int index);
		~TcpConnection();

		bool isConnected() const { return _status == kConnected; }
		bool isDisconnected() const { return _status == kDisconnected; }

		int getIndex() const { return _index; }
		EventLoop* getLooper() const { return _loop; }

		void send(const NetString& data);
		void send(const char* data, size_t len);

		void shutdown();
		void forceClose();

	protected:  /// Friend Methods
		void changeStatus(int status);
		void setObserver(ConnectionObserver* observer) { _observer = observer; }

		friend class TcpServer;
		friend class TcpClient;

	protected:  /// Looper Thread Methods
		void sendInLoop(const void* data, size_t len);
		ssize_t readInLoop();
		ssize_t writeInLoop(const char* buffer, size_t length);

		void handleReadInLoop();
		void handleWriteInLoop();
		void handleErrorInLoop();
		void handleCloseInLoop();

		void onConnectEstablishedInLoop();
		void onConnectDestroyedInLoop();

	protected:
		int _index;
		EventLoop* _loop;
		ConnectionObserver* _observer;
		std::unique_ptr<Socket> _socket;
		std::atomic_int _status;

		Buffer _inputBuffer;
		Buffer _outputBuffer;
	};


}

#endif //EXAMPLE_CONNECTION_H
