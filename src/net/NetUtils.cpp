//
// Created by beadle on 3/19/17.
//

#include "NetUtils.h"
#include "base/Logging.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>


namespace saf
{
	namespace net
	{
		void fromIpPort(const char* ip, uint16_t port, struct sockaddr_in* addr)
		{
			addr->sin_family = AF_INET;
			addr->sin_port = hostToNetwork16(port);
			if (::inet_pton(AF_INET, ip, &addr->sin_addr) <= 0)
			{
				// TODO: log error
			}
		}

	}
}