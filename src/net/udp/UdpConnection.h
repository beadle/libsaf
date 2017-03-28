//
// Created by beadle on 3/27/17.
//

#ifndef EXAMPLE_UDPCONNECION_H
#define EXAMPLE_UDPCONNECION_H

#include <string>
#include <memory>

#include "net/InetAddress.h"
#include "net/Buffer.h"
#include "net/Connection.h"
#include "net/Types.h"


namespace saf
{
	class EventLoop;
	class UdpConnection;

	class UdpConnection : public Connection
	{
	public: /// Thread-Safed Methods
		~UdpConnection();

	protected:  /// Looper Thread Methods
		UdpConnection(EventLoop* loop,
					  const std::string& key,
					  const InetAddress& addr);

		void setSenderFd(int fd) { _senderFd = fd; }

		void sendInLoop(const char* data, size_t length);
		void handleReadInLoop(const char *data, size_t length);
		void handleWriteInLoop();

		void onConnectEstablishedInLoop();
		void onConnectDestroyedInLoop();

		friend class UdpServer;

	private:
		bool _writing;
		int _senderFd;

		Buffer _inputBuffer;
		Buffer _outputBuffer;
	};

}

#endif //EXAMPLE_UDPCONNECION_H
