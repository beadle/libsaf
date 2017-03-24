//
// Created by beadle on 3/23/17.
//

#ifndef EXAMPLE_THREADPOOL_H
#define EXAMPLE_THREADPOOL_H

#include <vector>
#include <functional>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>


namespace saf
{

	class ThreadPool
	{
	public:
		typedef std::function<void()> Task;

	public:
		ThreadPool();

		void start(size_t count);
		void stop();
		void run(Task&& task);

	protected:
		void work();

	private:
		bool _stop;
		std::mutex _mutex;
		std::condition_variable _condition;

		std::queue<Task> _tasks;
		std::vector<std::thread> _workers;
	};
}

#endif //EXAMPLE_THREADPOOL_H
