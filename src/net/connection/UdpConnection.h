//
// Created by beadle on 3/23/17.
//

#ifndef EXAMPLE_UDPCONNECTION_H
#define EXAMPLE_UDPCONNECTION_H

#include "net/Connection.h"


namespace saf
{

	class UdpConnection : public Connection
	{
	public:
		UdpConnection(EventLoop* loop, Socket* socket, int index);
		~UdpConnection();

		NetProtocal getProtocal() const { return NetProtocal::UDP; }
	};

}

#endif //EXAMPLE_UDPCONNECTION_H
