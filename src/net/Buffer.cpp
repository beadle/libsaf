//
// Created by beadle on 3/14/17.
//

#include <unistd.h>
#include "Buffer.h"
#include "InetAddress.h"


namespace saf
{
	const size_t gExtraBufferLen = 65536;

	ssize_t Buffer::readFd(int fd)
	{
		// saved an ioctl()/FIONREAD call to tell how much to read
		char extrabuf[gExtraBufferLen];
		struct iovec vec[2];
		const size_t writable = writableBytes();
		vec[0].iov_base = begin() + _writerIndex;
		vec[0].iov_len = writable;
		vec[1].iov_base = extrabuf;
		vec[1].iov_len = gExtraBufferLen;
		// when there is enough space in this buffer, don't read into extrabuf.
		// when extrabuf is used, we read 128k-1 bytes at most.
		const int iovcnt = (writable < gExtraBufferLen) ? 2 : 1;
		const ssize_t n = ::readv(fd, vec, iovcnt);
		if (n < 0)
		{

		}
		else if (static_cast<size_t>(n) <= writable)
		{
			_writerIndex += n;
		}
		else
		{
			_writerIndex = _buffer.size();
			append(extrabuf, n - writable);
		}
		return n;
	}

	ssize_t Buffer::readFrom(int fd, InetAddress &addr)
	{
		size_t readLen = 65536;
		socklen_t socklen = sizeof(sockaddr_in);
		ensureWritableBytes(readLen);
		const ssize_t n = ::recvfrom(fd, beginWrite(), readLen, 0, addr.getSockAddrRaw(), &socklen);
		if (n < 0)
		{

		}
		else
		{
			_writerIndex += n;
		}
		return n;
	}
}