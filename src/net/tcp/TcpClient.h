//
// Created by beadle on 3/24/17.
//

#ifndef EXAMPLE_TCP_CLIENT_H
#define EXAMPLE_TCP_CLIENT_H

#include <memory>
#include <atomic>

#include "net/InetAddress.h"
#include "net/Types.h"
#include "net/Client.h"


namespace saf
{
	class TcpConnector;
	class EventLoop;
	class TcpConnection;
	class Socket;

	class TcpClient : public Client
	{
	public:
		TcpClient(EventLoop* loop);
		~TcpClient();

	public:  /// Thread-Safed Methods
		bool isConnected() const;
		TcpConnector* getConnector() const { return _connector.get(); }

		void connect(const InetAddress&);
		void disconnect();

		ConnectionPtr getConnection() const;

	protected:  /// Looper Thread Methods
		bool newConnectionInLoop(std::unique_ptr<Socket> &socket);
		void removeConnectionInLoop(const ConnectionPtr &conn);

	protected:
		void onClosedInConnection(const ConnectionPtr&);

	private:
		std::shared_ptr<TcpConnector> _connector;
		std::shared_ptr<TcpConnection> _connection;
	};

}

#endif //EXAMPLE_CLIENT_H
