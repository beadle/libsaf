//
// Created by beadle on 3/17/17.
//
#include <thread>
#include <iostream>

#include "core.h"

void thread1_func(saf::EventLoop* loop)
{
	loop->runInLoop([](){
		std::cout << "Thread1 func" << std::endl;
	});
}

void thread2_func(saf::EventLoop* loop)
{
	loop->runInLoop([](){
		std::cout << "Thread2 func" << std::endl;
	});
}

int main()
{
	saf::EventLoop loop;

	std::thread thread1(thread1_func, &loop);
	std::thread thread2(thread2_func, &loop);

	loop.run();

	return 0;
}