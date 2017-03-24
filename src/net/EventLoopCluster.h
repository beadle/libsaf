//
// Created by beadle on 3/23/17.
//

#ifndef EXAMPLE_EVENTPOOLCLUSTER_H
#define EXAMPLE_EVENTPOOLCLUSTER_H

#include <memory>
#include <vector>
#include <thread>
#include <atomic>


namespace saf
{
	class EventLoop;
	class ThreadPool;

	class EventLoopCluster
	{
		class Pair
		{
		public:
			Pair();
			~Pair();

			EventLoop* getLoop() const { return _loop.get(); }

			void start();
			void stop();

		protected:
			void work();

		private:
			std::unique_ptr<EventLoop> _loop;
			std::unique_ptr<std::thread> _thread;
		};

	public:
		EventLoopCluster();

		void start(size_t count);
		void stop();

		EventLoop* getNextLoop();

	private:
		std::atomic_int _counter;
		std::vector<std::unique_ptr<Pair> > _pairs;
	};

}

#endif //EXAMPLE_EVENTPOOLCLUSTER_H
