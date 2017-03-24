//
// Created by beadle on 3/14/17.
//

#include <unistd.h>
#include "Buffer.h"


namespace saf
{
	ssize_t Buffer::readFd(int fd)
	{
		// saved an ioctl()/FIONREAD call to tell how much to read
		char extrabuf[65536];
		struct iovec vec[2];
		const size_t writable = writableBytes();
		vec[0].iov_base = begin() + _writerIndex;
		vec[0].iov_len = writable;
		vec[1].iov_base = extrabuf;
		vec[1].iov_len = sizeof extrabuf;
		// when there is enough space in this buffer, don't read into extrabuf.
		// when extrabuf is used, we read 128k-1 bytes at most.
		const int iovcnt = (writable < sizeof extrabuf) ? 2 : 1;
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
}