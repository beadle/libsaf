//
// Created by beadle on 3/17/17.
//
#include <fcntl.h>
#include <iostream>

#include "FileUtils.h"

namespace saf
{
	namespace file
	{
		bool create_directories(const std::string& dirs)
		{
			auto errorno = ::system(("mkdir -p " + dirs).c_str());
			return errorno != -1;
		}

		AppendFile::AppendFile(const std::string& name):
			_fd(::fopen(name.c_str(), "a")),
			_writtenBytes(0)
		{
		}

		AppendFile::~AppendFile()
		{
			::fclose(_fd);
		}

		void AppendFile::append(const char* line, size_t len)
		{
			size_t n = write(line, len);
			size_t remain = len - n;
			while (remain > 0)
			{
				size_t x = write(line + n, remain);
				if (x == 0)
				{
					int err = ferror(_fd);
					if (err)
					{
						fprintf(stderr, "AppendFile::append() failed %d\n", err);
					}
					break;
				}
				n += x;
				remain = len - n;
			}
			_writtenBytes += len;
		}

		void AppendFile::flush()
		{
			::fflush(_fd);
		}

		size_t AppendFile::write(const char* line, size_t len)
		{
			return ::fwrite(line, 1, len, _fd);
		}

	}
}