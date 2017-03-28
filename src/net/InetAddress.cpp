//
// Created by beadle on 3/19/17.
//

#include "InetAddress.h"
#include "NetUtils.h"
#include "base/Logging.h"
#include <string.h>
#include <arpa/inet.h>

namespace saf
{
	InetAddress::InetAddress()
	{
		bzero(&_addr, sizeof _addr);
	}

	InetAddress::InetAddress(uint16_t port)
	{
		bzero(&_addr, sizeof _addr);
		_addr.sin_family = AF_INET;
		in_addr_t ip = INADDR_ANY;
		_addr.sin_addr.s_addr = net::hostToNetwork32(ip);
		_addr.sin_port = net::hostToNetwork16(port);
	}

	InetAddress::InetAddress(const std::string &ip, uint16_t port)
	{
		bzero(&_addr, sizeof _addr);
		_addr.sin_family = AF_INET;
		_addr.sin_port = net::hostToNetwork16(port);
		if (::inet_pton(AF_INET, ip.c_str(), &_addr.sin_addr) <= 0)
		{
			LOG_ERROR("InetAddress::InetAddress(const std::string &ip, uint16_t port)")
		}
	}

	InetAddress::InetAddress(const sockaddr_in &addr) :
		_addr(addr)
	{

	}

	const sockaddr* InetAddress::getSockAddr() const
	{
		return (const sockaddr*)&_addr;
	}

	sockaddr* InetAddress::getSockAddrRaw()
	{
		return (struct sockaddr*)&_addr;
	}

	uint16_t InetAddress::getPort() const
	{
		return net::networkToHost16(_addr.sin_port);
	}

	std::string InetAddress::toIp() const
	{
		char buff[64] = "";
		::inet_ntop(AF_INET, &_addr.sin_addr, buff, static_cast<socklen_t>(sizeof buff));
		return std::string(buff);
	}

	std::string InetAddress::toIpPort() const
	{
		size_t size = 64;
		char buff[size] = "";
		::inet_ntop(AF_INET, &_addr.sin_addr, buff, static_cast<socklen_t>(sizeof buff));
		size_t end = ::strlen(buff);
		uint16_t port = net::networkToHost16(_addr.sin_port);
		snprintf(buff + end, size - end, ":%u", port);
		return std::string(buff);
	}
}