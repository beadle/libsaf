//
// Created by beadle on 3/30/17.
//

#include "CountDownLatch.h"


namespace saf
{

	CountDownLatch::CountDownLatch(int count) :
		_count(count)
	{

	}

	void CountDownLatch::wait()
	{
		std::unique_lock<std::mutex> guard(_mutex);
		_cond.wait(guard, [this](){ return _count == 0; });
	}

	void CountDownLatch::countDown()
	{
		std::lock_guard<std::mutex> guard(_mutex);
		if (!--_count)
		{
			_cond.notify_all();
		}
	}

}