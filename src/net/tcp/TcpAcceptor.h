//
// Created by beadle on 3/14/17.
//

#ifndef LIBSAF_TCP_ACCEPTOR_H
#define LIBSAF_TCP_ACCEPTOR_H

#include <memory>
#include <functional>
#include <atomic>

#include "net/InetAddress.h"
#include "net/Types.h"
#include "net/Acceptor.h"


namespace saf
{
	class Socket;
	class EventLoop;
	class InetAddress;

	class TcpAcceptor : public Acceptor, public IOFdObserver
	{
	protected:
		typedef std::function<void(int, const InetAddress&)> AcceptCallback;

	public:
		~TcpAcceptor();

	public:  /// IOFdObserber Methods
		void onReadInIOFd(IOFd*);

	protected:
		TcpAcceptor(EventLoop *loop);

		void setAcceptCallback(const AcceptCallback& callback)
		{ _callback = std::move(callback); }

		const InetAddress& getAddress() const { return _addr; }

		void listenInLoop(const InetAddress&, bool);
		void stopInLoop();

		friend class TcpServer;

	private:
		void handleReadInLoop();

	private:
		int _idleFd;
		AcceptCallback _callback;
	};
}


#endif //LIBSAF_ACCEPTOR_H
