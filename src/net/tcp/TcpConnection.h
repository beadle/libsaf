//
// Created by beadle on 3/19/17.
//

#ifndef EXAMPLE_TCPCONNECTION_H
#define EXAMPLE_TCPCONNECTION_H

#include <memory>


namespace saf
{
	class Socket;
	class EventLoop;

	class TcpConnection
	{
	public:
		TcpConnection(EventLoop* loop, int sockfd, int index);

		int getIndex() const { return _index; }

	protected:
		void handleRead();
		void handleWrite();
		void handleError();
		void handleClose();

	private:
		int _index;
		EventLoop* _loop;
		std::unique_ptr<Socket> _socket;
	};

}


#endif //EXAMPLE_TCPCONNECTION_H
