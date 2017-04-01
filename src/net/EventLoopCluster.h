//
// Created by beadle on 3/23/17.
//

#ifndef EXAMPLE_EVENTPOOLCLUSTER_H
#define EXAMPLE_EVENTPOOLCLUSTER_H

#include <memory>
#include <vector>
#include <thread>
#include <atomic>

#include "base/CountDownLatch.h"


namespace saf
{
	class EventLoop;
	class ThreadPool;

	class EventLoopCluster
	{
		class Pair
		{
		public:
			Pair(EventLoopCluster* master);
			~Pair();

			EventLoop* getLoop() const { return _loop.get(); }

			void start();
			void stop();

		protected:
			void work();

		private:
			EventLoopCluster* _master;
			std::unique_ptr<EventLoop> _loop;
			std::unique_ptr<std::thread> _thread;
		};

	public:
		EventLoopCluster(EventLoop* master);

		void start(size_t count);
		void stop();

		EventLoop* getNextLoop();

	protected:
		void onReady(Pair* pair);

	private:
		EventLoop* _master;
		size_t _threadCount;
		std::atomic_int _counter;
		std::unique_ptr<CountDownLatch> _latch;
		std::vector<std::unique_ptr<Pair> > _pairs;
	};

}

#endif //EXAMPLE_EVENTPOOLCLUSTER_H
