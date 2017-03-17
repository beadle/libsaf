//
// Created by beadle on 3/17/17.
//

#ifndef EXAMPLE_FILEUTILS_H
#define EXAMPLE_FILEUTILS_H

#include <stdio.h>
#include <string>


namespace saf
{
	namespace file
	{
		extern bool create_directories(const std::string& dirs);

		class AppendFile
		{
		public:
			explicit AppendFile(const std::string& name);
			~AppendFile();

			void append(const char* line, size_t len);
			void flush();

		protected:
			size_t write(const char* line, size_t len);

		private:
			FILE* _fd;
			int	_writtenBytes;
		};
	}
}


#endif //EXAMPLE_FILEUTILS_H
