//
// Created by beadle on 3/17/17.
//

#include <sstream>
#include "Logging.h"
#include "AsyncLogger.h"


namespace saf
{
	static AsyncLogger gLogger("log/game.log");

	const char* LogLevelName[int(LogLevel::LEVEL_COUNT)] =
	{
		"DEBUG ",
		"INFO  ",
		"WARN  ",
		"ERROR ",
		"FATAL ",
	};

	void log(LogLevel level, const char* file, int line, const char* function, const char* content)
	{
		std::ostringstream stream;
		stream << LogLevelName[int(level)]
				   << file
				   << '(' << line << ") - "
				   << function
				   << ": " << content << '\n';
		gLogger.append(stream.str().c_str(), stream.str().size());
	}

	void startLogger()
	{
		gLogger.start();
	}

	void stopLogger()
	{
		gLogger.stop();
	}
}