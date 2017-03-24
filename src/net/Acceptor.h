//
// Created by beadle on 3/14/17.
//

#ifndef LIBSAF_ACCEPTOR_H
#define LIBSAF_ACCEPTOR_H

#include <memory>
#include <functional>

#include "InetAddress.h"
#include "Types.h"


namespace saf
{
	class Socket;
	class EventLoop;
	class InetAddress;

	class Acceptor
	{
	public:
		typedef std::function<void(int, const InetAddress&)> AcceptCallback;

	public:
		Acceptor(EventLoop *loop,
				 const InetAddress &addr,
				 NetProtocal protocal=NetProtocal::TCP,
				 bool reusePort=false);
		~Acceptor();

		void listen();

		void setAcceptCallback(const AcceptCallback& callback)
		{ _callback = std::move(callback); }

		const InetAddress& getAddress() const { return _addr; }

	protected:
		void handleRead();

	private:
		bool _listening;
		int _idleFd;
		EventLoop* _loop;
		std::unique_ptr<Socket> _socket;
		AcceptCallback _callback;
		InetAddress _addr;
	};
}


#endif //LIBSAF_ACCEPTOR_H
