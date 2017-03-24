//
// Created by beadle on 3/23/17.
//

#ifndef EXAMPLE_KCPCONNECTION_H
#define EXAMPLE_KCPCONNECTION_H

#include "net/Connection.h"


namespace saf
{

	class KcpConnection : public Connection
	{
	public:
		KcpConnection(EventLoop* loop, Socket* socket, int index);

		NetProtocal getProtocal() const { return NetProtocal::KCP; }
	};

}

#endif //EXAMPLE_KCPCONNECTION_H
