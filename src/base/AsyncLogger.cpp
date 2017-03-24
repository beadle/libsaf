//
// Created by beadle on 3/17/17.
//

#include <assert.h>
#include <chrono>

#include "AsyncLogger.h"


namespace saf
{
	LogFile::LogFile(const std::string& path):
		_file(NULL),
		_path(path)
	{
		auto pos = path.find_last_of('/');
		std::string dirs = path.substr(0, pos);

		file::create_directories(dirs);
		_file.reset(new file::AppendFile(_path));
	}

	LogFile::~LogFile()
	{
	}

	void LogFile::append(const char* line, size_t len)
	{
		_file->append(line, len);
	}

	void LogFile::flush()
	{
		_file->flush();
	}


	AsyncLogger::AsyncLogger(const std::string& path, float flushInterval):
		_running(false),
		_file(path),
		_interval(long(flushInterval * 1000)),
		_thread(nullptr)
	{
		_currBuffer.reset(new Buffer);
	}

	AsyncLogger::~AsyncLogger()
	{
		stop();
	}

	void AsyncLogger::append(const char* line, size_t len)
	{
		std::lock_guard<std::mutex> guard(_mutex);
		if (_currBuffer->avail() <= len)
		{
			_buffers.push_back(_currBuffer);
			_currBuffer.reset(new Buffer);

			_cond.notify_one();
		}
		_currBuffer->append(line, len);
	}

	void AsyncLogger::threadFunc()
	{
		BufferVector writeBuffers;
		BufferPtr emptyBuffer(new Buffer);

		while (_running)
		{
			assert(writeBuffers.empty());
			{
				std::unique_lock<std::mutex> guard(_mutex);
				if (_buffers.empty())
				{
					_cond.wait_for(guard, std::chrono::milliseconds(_interval));
				}
				writeBuffers.swap(_buffers);
				writeBuffers.push_back(_currBuffer);
				_currBuffer.swap(emptyBuffer);
			}

			int writtenLen = 0;
			for (int i = 0; i < writeBuffers.size(); ++i)
			{
				BufferPtr buffer = writeBuffers[i];
				writtenLen += buffer->length();
				_file.append(buffer->data(), buffer->length());
			}

			if (writtenLen > 0)
				_file.flush();

			emptyBuffer->reset();
			writeBuffers.clear();
		}
	}

	void AsyncLogger::start()
	{
		assert(!_running);

		_running = true;
		_thread.reset(new std::thread(std::bind(&AsyncLogger::threadFunc, this)));
	}

	void AsyncLogger::stop()
	{
		if (!_running)
			return;

		_running = false;
		_thread->join();
		_thread.reset();
	}

}