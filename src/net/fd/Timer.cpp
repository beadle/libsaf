//
// Created by beadle on 3/16/17.
//

#include "Timer.h"
#include "../../base/TimeUtils.h"


namespace saf
{
	Timer::Timer(int fd, long interval, TimerCallback&& callback) :
		_repeated(false),
		_fd(fd),
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