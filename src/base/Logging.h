//
// Created by beadle on 3/17/17.
//

#ifndef EXAMPLE_LOGGING_H
#define EXAMPLE_LOGGING_H

#include <string.h>
#include <errno.h>


namespace saf
{
	enum class LogLevel
	{
		DEBUG,
		INFO,
		WARN,
		ERROR,
		FATAL,
		COUNT,
	};

	extern void log(LogLevel level, const char* file, int line,
			const char* function, int error, const char* format, ...);

	extern const char* errnoToString(int error);

	class LoggerLauncher
	{
	public:
		LoggerLauncher();
		~LoggerLauncher();
	};


#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define LOG_DEBUG(__CONTENT__, ARGS...) saf::log(saf::LogLevel::DEBUG, __FILENAME__, __LINE__, __FUNCTION__, 0, __CONTENT__, ##ARGS);
#define LOG_INFO(__CONTENT__, ARGS...) saf::log(saf::LogLevel::INFO, __FILENAME__, __LINE__, nullptr, 0, __CONTENT__, ##ARGS);
#define LOG_WARN(__CONTENT__, ARGS...) saf::log(saf::LogLevel::WARN, __FILENAME__, __LINE__, nullptr, 0, __CONTENT__, ##ARGS);
#define LOG_ERROR(__CONTENT__, ARGS...) saf::log(saf::LogLevel::ERROR, __FILENAME__, __LINE__, nullptr, errno, __CONTENT__, ##ARGS);
#define LOG_FATAL(__CONTENT__, ARGS...) saf::log(saf::LogLevel::FATAL, __FILENAME__, __LINE__, nullptr, errno, __CONTENT__, ##ARGS);

#define INIT_LOGGER saf::LoggerLauncher __launch__;

}

#endif //EXAMPLE_LOGGING_H
