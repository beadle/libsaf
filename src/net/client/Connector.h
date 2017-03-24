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
	public:
		typedef std::function<bool(std::unique_ptr<Socket>&)> ConnectedCallback;

	public:
		Connector(EventLoop* loop, float retrySeconds);
		~Connector();

		void connect(const InetAddress &addr, NetProtocal protocal);
		void reconnect();
		void disconnect();

		const InetAddress& getAddress() const { return _addr; };
		NetProtocal getProtocal() const { return _protocal; }

		void setConnectedCallback(const ConnectedCallback& callback)
		{ _connectedCallback = callback; }

	protected:
		enum { kDisconnected, kConnecting, kConnected, };

		void changeStatus(int status) { _status = status; }

		void connectInLoop();
		void reconnectInLoop();
		void disconnectInLoop();

		void onConnecting();
		void onRetry();

		void handleWrite();
		void handleError();

		void detachFromEventPool();

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
