//
// Created by beadle on 3/14/17.
//

#ifndef LIBSAF_ACCEPTOR_H
#define LIBSAF_ACCEPTOR_H

#include <memory>
#include <functional>
#include <atomic>

#include "net/InetAddress.h"
#include "net/Types.h"


namespace saf
{
	class Socket;
	class EventLoop;
	class InetAddress;

	class TcpAcceptor
	{
	protected:
		typedef std::function<void(int, const InetAddress&)> AcceptCallback;

	public:
		~TcpAcceptor();

	protected:
		TcpAcceptor(EventLoop *loop,
				 const InetAddress &addr,
				 bool reusePort=false);

	protected:
		void setAcceptCallback(const AcceptCallback& callback)
		{ _callback = std::move(callback); }

		const InetAddress& getAddress() const { return _addr; }

		void listenInLoop();
		void stopInLoop();

		friend class TcpServer;

	private:
		void handleReadInLoop();

	private:
		bool _reusePort;
		bool _listening;
		int _idleFd;
		EventLoop* _loop;
		std::unique_ptr<Socket> _socket;
		AcceptCallback _callback;
		InetAddress _addr;
	};
}


#endif //LIBSAF_ACCEPTOR_H
