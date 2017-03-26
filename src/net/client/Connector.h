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
	class Socket;
	class EventLoop;

	class Connector : public std::enable_shared_from_this<Connector>
	{
	protected:
		typedef std::function<bool(std::unique_ptr<Socket>&)> ConnectedCallback;

	public:
		Connector(EventLoop* loop, float retrySeconds);
		~Connector();

	protected:
		void connect(const InetAddress &addr, NetProtocal protocal);
		void disconnect();

		void setConnectedCallback(const ConnectedCallback& callback)
		{ _connectedCallback = callback; }

		friend class Client;

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
		InetAddress _addr;
		NetProtocal _protocal;
		EventLoop* _loop;
		std::atomic_bool _stopping;
		float _retrySeconds;
		ConnectedCallback _connectedCallback;

		std::atomic_int _status;
		std::unique_ptr<Socket> _socket;

	};

}

#endif //EXAMPLE_CONNECTOR_H
