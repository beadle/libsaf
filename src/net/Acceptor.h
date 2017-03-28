//
// Created by beadle on 3/28/17.
//

#ifndef EXAMPLE_ACCEPTOR_H
#define EXAMPLE_ACCEPTOR_H

#include <memory>
#include "InetAddress.h"


namespace saf
{
	class Socket;
	class EventLoop;

	class Acceptor
	{
	public:
		Acceptor(EventLoop *loop);
		virtual ~Acceptor();

		Socket* getSocket() const { return _socket.get(); }
		const InetAddress& getAddress() const { return _addr; }

		virtual void listenInLoop(const InetAddress&, bool) = 0;
		virtual void stopInLoop() = 0;

	protected:
		bool _listening;
		EventLoop* _loop;
		InetAddress _addr;
		std::unique_ptr<Socket> _socket;
	};

}

#endif //EXAMPLE_ACCEPTOR_H
