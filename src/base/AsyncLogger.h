//
// Created by beadle on 3/17/17.
//

#ifndef EXAMPLE_ASYNCLOGGER_H
#define EXAMPLE_ASYNCLOGGER_H

#include <memory>
#include <string.h>
#include <vector>
#include <mutex>
#include <condition_variable>

#include "Thread.h"
#include "FileUtils.h"


namespace saf
{
	template<int SIZE>
	class FixedBuffer
	{
	public:
		FixedBuffer() :_curr(_data) {}

		void append(const char* buf, size_t len)
		{
			if (avail() > len)
			{
				::memcpy(_curr, buf, len);
				_curr += len;
			}
		}

		const char* data() const { return _data; }

		size_t length() const { return static_cast<size_t>(_curr - _data); }
		size_t avail() const { return static_cast<size_t>(end() - _curr); }
		void reset() { _curr = _data; }

		std::string toString() { return std::string(_data, length()); }

	public:  // exposed to function log only
		char* current() { return _curr; }
		void add(size_t len) { _curr += len; }

	protected:
		const char* end() const { return _data + sizeof _data; }

	private:
		char _data[SIZE];
		char* _curr;
	};


	class LogFile
	{
	public:
		explicit LogFile(const std::string& path);
		~LogFile();

		void append(const char* line, size_t len);
		void flush();

	private:
		std::string	_path;
		std::shared_ptr<file::AppendFile> _file;
	};


	class AsyncLogger
	{
	public:
		explicit AsyncLogger(const std::string& path, float flushInterval=0.25);
		~AsyncLogger();

		void append(const char* line, size_t len);
		void start();
		void stop();

	protected:
		void threadFunc();

	private:
		typedef FixedBuffer<128*1024> Buffer;
		typedef std::shared_ptr<Buffer>	BufferPtr;
		typedef std::vector<BufferPtr> BufferVector;

		bool _running;
		long _interval;
		std::mutex _mutex;
		std::condition_variable _cond;

		LogFile _file;
		Thread _thread;

		BufferPtr _currBuffer;
		BufferVector _buffers;
	};
}

#endif //EXAMPLE_ASYNCLOGGER_H
