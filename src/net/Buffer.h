//
// Created by beadle on 3/14/17.
//

#ifndef LIBSAF_BUFFER_H
#define LIBSAF_BUFFER_H

#include <assert.h>
#include <string>
#include <vector>
#include <string.h>

#include "net/NetUtils.h"
#include "net/NetString.h"


namespace saf
{
	class InetAddress;

	/// A buffer class modeled after org.jboss.netty.buffer.ChannelBuffer
	///
	/// @code
	/// +-------------------+------------------+------------------+
	/// | prependable bytes |  readable bytes  |  writable bytes  |
	/// |                   |     (CONTENT)    |                  |
	/// +-------------------+------------------+------------------+
	/// |                   |                  |                  |
	/// 0      <=      readerIndex   <=   writerIndex    <=     size
	/// @endcode
	class Buffer
	{
	public:
		static const size_t kCheapPrepend = 0;
		static const size_t kInitialSize = 1024;

		explicit Buffer(size_t initialSize=kInitialSize):
			_buffer(kCheapPrepend + initialSize),
			_readerIndex(kCheapPrepend),
			_writerIndex(kCheapPrepend)
		{
			assert(readableBytes() == 0);
			assert(writableBytes() == initialSize);
			assert(prependableBytes() == kCheapPrepend);
		}

		size_t readableBytes() const { return _writerIndex - _readerIndex; }
		size_t writableBytes() const { return _buffer.size() - _writerIndex; }
		size_t prependableBytes() const { return _readerIndex; }

		inline void retrieve(size_t len)
		{
			assert(len <= readableBytes());
			if (len < readableBytes())
			{
				_readerIndex += len;
			}
			else
			{
				retrieveAll();
			}
		}

		inline void retrieveAll()
		{
			_readerIndex = kCheapPrepend;
			_writerIndex = kCheapPrepend;
		}

		template<size_t SIZE>
		void retrieve()
		{
			retrieve(SIZE);
		}

		std::string retrieveAllAsString()
		{
			return retrieveAsString(readableBytes());;
		}

		std::string retrieveAsString(size_t len)
		{
			assert(len <= readableBytes());
			std::string result(peek(), len);
			retrieve(len);
			return result;
		}

		NetString retrieveAllAsNetString()
		{
			NetString str(peek(), readableBytes());
			retrieveAll();
			return str;
		}

		NetString retrieveAsNetString(size_t len)
		{
			assert(len <= readableBytes());
			NetString result(peek(), len);
			retrieve(len);
			return result;
		}

		void append(const char* data, size_t len)
		{
			ensureWritableBytes(len);
			std::copy(data, data + len, beginWrite());
			_writerIndex += len;
		}

		void append(const void* data, size_t len)
		{
			append(static_cast<const char*>(data), len);
		}

		void append(const std::string& data)
		{
			append(data.c_str(), data.size());
		}

		void appendInt64(int64_t x)
		{
			int64_t be64 = net::hostToNetwork64(x);
			append(&be64, sizeof be64);
		}

		void appendInt32(int32_t x)
		{
			int32_t be32 = net::hostToNetwork32(x);
			append(&be32, sizeof be32);
		}

		void appendInt16(int16_t x)
		{
			int16_t be16 = net::hostToNetwork16(x);
			append(&be16, sizeof be16);
		}

		int64_t readInt64()
		{
			int64_t result = peekInt64();
			retrieve<64>();
			return result;
		}

		int32_t readInt32()
		{
			int32_t result = peekInt32();
			retrieve<32>();
			return result;
		}

		int16_t readInt16()
		{
			int16_t result = peekInt16();
			retrieve<16>();
			return result;
		}

		int8_t readInt8()
		{
			int8_t result = peekInt8();
			retrieve<8>();
			return result;
		}

		void appendInt8(int8_t x)
		{
			append(&x, sizeof x);
		}

		inline const char* peek() const { return begin() + _readerIndex; }

		int64_t peekInt64() const
		{
			assert(readableBytes() >= sizeof(int64_t));
			int64_t be64 = 0;
			::memcpy(&be64, peek(), sizeof be64);
			return net::networkToHost64(be64);
		}

		int32_t peekInt32() const
		{
			assert(readableBytes() >= sizeof(int32_t));
			int32_t be32 = 0;
			::memcpy(&be32, peek(), sizeof be32);
			return net::networkToHost32(be32);
		}

		int16_t peekInt16() const
		{
			assert(readableBytes() >= sizeof(int16_t));
			int16_t be16 = 0;
			::memcpy(&be16, peek(), sizeof be16);
			return net::networkToHost16(be16);
		}

		int8_t peekInt8() const
		{
			assert(readableBytes() >= sizeof(int8_t));
			int8_t x = *peek();
			return x;
		}

		std::string asString() const { return std::string(peek(), readableBytes()); }
		NetString asNetString() const { return NetString(peek(), readableBytes()); }

		/// Read data directly into buffer.
		///
		/// It may implement with readv(2)
		/// @return result of read(2)
		ssize_t readFd(int fd);

		/// Recv data directly into buffer.
		///
		/// It may implement with recvfrom(2)
		/// @return result of recvfrom(2)
		ssize_t readFrom(int fd, InetAddress& addr);

	protected:
		inline char* begin() { return &*_buffer.begin(); }
		inline const char* begin() const { return &*_buffer.begin(); }
		inline char* beginWrite() { return begin() + _writerIndex; }
		inline const char* beginWrite() const { return begin() + _writerIndex; }

		void ensureWritableBytes(size_t len)
		{
			if (writableBytes() < len)
			{
				makeSpace(len);
			}
			assert(writableBytes() >= len);
		}

		void makeSpace(size_t len)
		{
			if (writableBytes() + prependableBytes() < len + kCheapPrepend) {
				// FIXME: move readable data
				_buffer.resize(_writerIndex + len);
			} else {
				// move readable data to the front, make space inside buffer
				assert(kCheapPrepend < _readerIndex);
				size_t readable = readableBytes();
				std::copy(begin() + _readerIndex,
						  begin() + _writerIndex,
						  begin() + kCheapPrepend);
				_readerIndex = kCheapPrepend;
				_writerIndex = _readerIndex + readable;
				assert(readable == readableBytes());
			}
		}
	private:
		std::vector<char> _buffer;
		size_t _readerIndex;
		size_t _writerIndex;
	};
}



#endif //LIBSAF_BUFFER_H
