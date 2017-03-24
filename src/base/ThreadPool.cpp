//
// Created by beadle on 3/23/17.
//

#include <assert.h>
#include "ThreadPool.h"


namespace saf
{
	ThreadPool::ThreadPool() :
		_stop(false)
	{
	}

	void ThreadPool::run(Task &&task)
	{
		if (_stop)
		{
			return;
		}

		if (_workers.empty())
		{
			task();
			return;
		}

		std::lock_guard<std::mutex> guard(_mutex);
		_tasks.push(std::move(task));
		_condition.notify_one();
	}

	void ThreadPool::start(size_t count)
	{
		assert(_workers.empty());

		for (size_t i=0; i<count; ++i)
			_workers.emplace_back(std::bind(&ThreadPool::work, this));
	}

	void ThreadPool::stop()
	{
		{
			std::lock_guard<std::mutex> guard(_mutex);
			_stop = true;
			_condition.notify_all();
		}

		for (auto& worker : _workers)
			worker.join();
		_workers.clear();
	}

	void ThreadPool::work()
	{
		Task task = nullptr;
		{
			std::unique_lock<std::mutex> guard(_mutex);
			_condition.wait(guard, [this]() { return _stop || !_tasks.empty(); });
			if (_stop) return;

			task = std::move(_tasks.front());
			_tasks.pop();
		}
		task();
	}

}