//
// Created by beadle on 3/27/17.
//

#ifndef EXAMPLE_UDPACCEPTOR_H
#define EXAMPLE_UDPACCEPTOR_H

#include <memory>
#include <functional>
#include "net/Buffer.h"
#include "net/InetAddress.h"
#include "net/Acceptor.h"
#include "net/Types.h"


namespace saf
{
	class IOFd;
	class Socket;
	class EventLoop;

	class UdpAcceptor : public Acceptor, public IOFdObserver
	{
	protected:
		typedef std::function<void(InetAddress&, Buffer*)> ReceMessageCallback;

	public:
		~UdpAcceptor();

	public:  /// IOFdObserber Methods
		void onReadInIOFd(IOFd*);
		void onWriteInIOFd(IOFd*);
		void onErrorInIOFd(IOFd*);
		void onCloseInIOFd(IOFd*);

	protected:
		UdpAcceptor(EventLoop *loop);

		void setRecvMessageCallback(const ReceMessageCallback& callback)
		{ _recvMessageCallback = callback; }

		void listenInLoop(const InetAddress&, bool);
		void stopInLoop();

		friend class UdpServer;

	private:
		void handleReadInLoop();
		void handleWriteInLoop();
		void handleErrorInLoop();
		void handleCloseInLoop();

	private:
		ReceMessageCallback _recvMessageCallback;

		Buffer _inputBuffer;
		Buffer _outputBuffer;
	};

}

#endif //EXAMPLE_UDPACCEPTOR_H
