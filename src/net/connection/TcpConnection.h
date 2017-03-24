//
// Created by beadle on 3/19/17.
//

#ifndef EXAMPLE_TCPCONNECTION_H
#define EXAMPLE_TCPCONNECTION_H

#include "net/Connection.h"


namespace saf
{

	class TcpConnection : public Connection
	{
	public:
		TcpConnection(EventLoop* loop, Socket* socket, int index);
		~TcpConnection();

		NetProtocal getProtocal() const { return NetProtocal::TCP; }
	};

}


#endif //EXAMPLE_TCPCONNECTION_H
