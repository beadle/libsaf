//
// Created by beadle on 3/17/17.
//

#ifndef EXAMPLE_LOGGING_H
#define EXAMPLE_LOGGING_H

#include <string.h>


namespace saf
{
	enum class LogLevel
	{
		DEBUG,
		INFO,
		WARN,
		ERROR,
		FATAL,
		LEVEL_COUNT,
	};

	extern void log(LogLevel level, const char* file, int line, const char* function, const char* content);

	class LoggerLauncher
	{
	public:
		LoggerLauncher();
		~LoggerLauncher();
	};


#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define LOG_DEBUG(__CONTENT__) 	saf::log(saf::LogLevel::DEBUG, __FILENAME__, __LINE__, __FUNCTION__, __CONTENT__);
#define LOG_INFO(__CONTENT__) 	saf::log(saf::LogLevel::INFO, __FILENAME__, __LINE__, __FUNCTION__, __CONTENT__);
#define LOG_WARN(__CONTENT__) 	saf::log(saf::LogLevel::WARN, __FILENAME__, __LINE__, __FUNCTION__, __CONTENT__);
#define LOG_ERROR(__CONTENT__) 	saf::log(saf::LogLevel::ERROR, __FILENAME__, __LINE__, __FUNCTION__, __CONTENT__);
#define LOG_FATAL(__CONTENT__) 	saf::log(saf::LogLevel::FATAL, __FILENAME__, __LINE__, __FUNCTION__, __CONTENT__);

#define INIT_LOGGER saf::LoggerLauncher __launch__;

}

#endif //EXAMPLE_LOGGING_H
