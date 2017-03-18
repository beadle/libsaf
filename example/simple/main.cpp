//
// Created by beadle on 3/17/17.
//
#include <random>
#include <thread>
#include <unistd.h>
#include <iostream>

#include "core.h"

void test_timer(saf::EventLoop* loop)
{
	for (int i=0; i<50; ++i)
	{
		float delay = 2.0f + (float)((double) rand() / (RAND_MAX)) * 8.0f;
		loop->addTimer(delay, [delay](){
			std::cout << "once timer(" << delay << ")" << std::endl;
		});
	}

	for (int i=0; i<5; ++i)
	{
		loop->addTimer(4.0, [loop](){
			int fd = 1 + (int)(((double) rand() / (RAND_MAX)) * 40.0f);
			loop->cancelTimer(fd);
			std::cout << "cancel timer(" << fd << ")" << std::endl;
		});
	}

	for (int i=0; i<5; ++i)
	{
		loop->addTimer(5.0, [loop](){
			float delay = 2.0f + (float)((double) rand() / (RAND_MAX)) * 3.0f;
			loop->addTimer(delay, [delay](){
				std::cout << "inner add timer(" << delay + 5.0 << ")" << std::endl;
			});
		});
	}

	int interval = 3;
	loop->addTimer(interval, [interval](){
		std::cout << "repeated timer(" << interval << ")" << std::endl;
	}, true);

}

void thread1_func(saf::EventLoop* loop)
{
	loop->runInLoop([loop](){
		std::cout << "Thread1 func" << std::endl;
	});
}

void thread2_func(saf::EventLoop* loop)
{
	test_timer(loop);
}

int main()
{
	INIT_LOGGER;

//	for (int i=0; i<10; ++i)
//		LOG_DEBUG("fuck you.");
//	::sleep(2);
//	for (int i=0; i<10; ++i)
//		LOG_WARN("fuck you.");

	saf::EventLoop loop;
	std::thread thread1(thread1_func, &loop);
	std::thread thread2(thread2_func, &loop);

//	test_timer(loop);

	loop.run();

	return 0;
}