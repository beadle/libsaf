//
// Created by beadle on 3/17/17.
//

#ifndef EXAMPLE_THREAD_H
#define EXAMPLE_THREAD_H

#include <functional>
#include <string>
#include <atomic>
#include <thread>
#include <memory>


namespace saf
{

	class Thread
	{
	public:
		typedef std::function<void ()> ThreadFunc;

		explicit Thread(ThreadFunc&& threadFunc, const std::string& name="");
		~Thread();

		void start();
		void join();

	private:
		std::unique_ptr<std::thread> _thread;
		ThreadFunc _threadFunc;
		std::string _name;

	private:
		static std::atomic<int>	s_threadCount;
	};

}

#endif //EXAMPLE_THREAD_H
