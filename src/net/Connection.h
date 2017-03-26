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

	class Connection : public std::enable_shared_from_this<Connection>
	{
	public:
		enum {
			kDisconnected,
			kConnecting,
			kConnected,
			kDisconnecting,
		};

	public: /// Thread-Safed Methods
		Connection(EventLoop* loop, Socket* socket, int index);
		~Connection();

		bool isConnected() const { return _status == kConnected; }
		bool isDisconnected() const { return _status == kDisconnected; }

		int getIndex() const { return _index; }
		EventLoop* getLooper() const { return _loop; }
		virtual NetProtocal getProtocal() const = 0;

		void send(const NetString& data);
		void send(const char* data, size_t len);

		void shutdown();
		void forceClose();

	protected:  /// Friend Methods
		void changeStatus(int status);
		void setObserver(ConnectionObserver* observer) { _observer = observer; }

		friend class Server;
		friend class Client;

	protected:  /// Looper Thread Methods
		void sendInLoop(const void* data, size_t len);

		void handleReadInLoop();
		void handleWriteInLoop();
		void handleErrorInLoop();
		void handleCloseInLoop();

		void onConnectEstablishedInLoop();
		void onConnectDestroyedInLoop();

	private:
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
