//
// Created by beadle on 3/19/17.
//

#ifndef EXAMPLE_INETADDRESS_H
#define EXAMPLE_INETADDRESS_H

#include <string>
#include <netinet/in.h>

namespace saf
{

	class InetAddress
	{
	public:
		explicit InetAddress(uint16_t port);
		explicit InetAddress(const sockaddr_in& addr);

		InetAddress(const std::string& ip, uint16_t port);

		sa_family_t getFamily() const { return _addr.sin_family; }
		uint16_t getPortNetEndian() const { return _addr.sin_port; }
		uint16_t getPort() const;
		const sockaddr* getSockAddr() const;

		std::string toIp() const;
		std::string toIpPort() const;

	private:
		struct sockaddr_in _addr;
	};

}

#endif //EXAMPLE_INETADDRESS_H
