//
// Created by beadle on 3/23/17.
//

#include "EventLoopCluster.h"
#include "net/EventLoop.h"
#include "base/ThreadPool.h"
#include "base/Logging.h"


namespace saf
{
	EventLoopCluster::Pair::Pair() :
		_loop(nullptr),
		_thread(nullptr)
	{

	}

	EventLoopCluster::Pair::~Pair()
	{
		assert(!_loop);
		assert(!_thread);
	}

	void EventLoopCluster::Pair::start()
	{
		assert(!_thread);
		auto threadFunc = std::bind(&EventLoopCluster::Pair::work, this);
		_thread.reset(new std::thread(threadFunc));
	}

	void EventLoopCluster::Pair::stop()
	{
		assert(_loop);
		_loop->stop();

		_thread->join();
		_thread.reset();
	}

	void EventLoopCluster::Pair::work()
	{
		assert(!_loop);
		_loop.reset(new EventLoop());
		_loop->start();
		_loop.reset();
	}


	EventLoopCluster::EventLoopCluster() :
		_counter(0)
	{

	}

	void EventLoopCluster::start(size_t count)
	{
		_pairs.clear();

		for (size_t i=0; i<count; ++i)
		{
			_pairs.emplace_back(new Pair());
			_pairs.back()->start();
		}
	}

	void EventLoopCluster::stop()
	{
		for (auto& pair : _pairs)
			pair->stop();

		_pairs.clear();
	}

	EventLoop* EventLoopCluster::getNextLoop()
	{
		assert(!_pairs.empty());

		auto index = ++_counter % _pairs.size();
		return _pairs[index]->getLoop();
	}
}