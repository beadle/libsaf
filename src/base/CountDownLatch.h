//
// Created by beadle on 3/30/17.
//

#ifndef EXAMPLE_COUNTDOWNLATCH_H
#define EXAMPLE_COUNTDOWNLATCH_H


#include <mutex>
#include <condition_variable>

namespace saf
{

	class CountDownLatch
	{
	public:
		explicit CountDownLatch(int count);

		void wait();
		void countDown();

	private:
		int _count;
		std::mutex _mutex;
		std::condition_variable _cond;
	};

}

#endif //EXAMPLE_COUNTDOWNLATCH_H
