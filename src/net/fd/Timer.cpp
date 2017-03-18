//
// Created by beadle on 3/16/17.
//

#include "Timer.h"
#include "base/TimeUtils.h"


namespace saf
{
	Timer::Timer(int fd, long interval, TimerCallback&& callback) :
		Fd(fd),
		_repeated(false),
		_interval(interval),
		_timestamp(time::timestamp() + interval),
		_callback(std::move(callback))
	{

	}

	void Timer::handleTimeout()
	{
		_callback();
	}
}