//
// Created by beadle on 3/24/17.
//

#ifndef EXAMPLE_CONNECTOR_H
#define EXAMPLE_CONNECTOR_H

#include <memory>
#include <atomic>

#include "net/InetAddress.h"
#include "net/Types.h"


namespace saf
{
	class Client;
	class Socket;
	class EventLoop;

	class TcpConnector : public std::enable_shared_from_this<TcpConnector>
	{
	protected:
		typedef std::function<bool(std::unique_ptr<Socket>&)> ConnectedCallback;

	public:
		~TcpConnector();

		Socket* getSocket() { return _socket.get(); }

	protected:
		TcpConnector(EventLoop* loop, Client* master);

	protected:
		void connect(const InetAddress &addr);
		void disconnect();

		void setConnectedCallback(const ConnectedCallback& callback)
		{ _connectedCallback = callback; }

		friend class TcpClient;

	private:
		enum { kDisconnected, kConnecting, kConnected, };

		void connectInLoop();
		void disconnectInLoop();

		void onConnectingInLoop();
		void onRetryInLoop();

		void handleWriteInLoop();
		void handleErrorInLoop();
		void handleCloseInLoop();

	private:
		void changeStatus(int status) { _status = status; }
		void resetSocket();

	private:
		Client* _master;
		InetAddress _addr;
		EventLoop* _loop;
		std::atomic_bool _stopping;
		ConnectedCallback _connectedCallback;

		std::atomic_int _status;
		std::unique_ptr<Socket> _socket;

	};

}

#endif //EXAMPLE_CONNECTOR_H
