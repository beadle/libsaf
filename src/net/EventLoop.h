//
// Created by beadle on 3/16/17.
//

#ifndef LIBSAF_EVENTLOOP_H
#define LIBSAF_EVENTLOOP_H

#include <assert.h>
#include <atomic>
#include <memory>
#include <mutex>
#include <vector>

#include "base/CurrentThread.h"


namespace saf
{
	class Fd;
	class Poller;

	class EventLoop
	{
	public:
		typedef std::function<void()> Functor;

	public:
		EventLoop();
		~EventLoop();

		void run();
		void quit();

		void runInLoop(Functor&& functor);

	public:  // exposed to fd objects(Socket, Timer...)
		bool hasFd(Fd* fd);
		void updateFd(Fd* fd);
		void removeFd(Fd* fd);

	protected:
		bool isInLoopThread() { return _threadId == CurrentThread::tid(); }
		void assertInLoopThread() { assert(isInLoopThread()); }

		void wakeup();
		void handleWakeupRead();

		void runFunctors();
		void queueInLoop(Functor&& functor);

	private:
		const pid_t _threadId;

		std::atomic_bool _quit;
		std::atomic_bool _looping;
		std::atomic_bool _handling;

		Fd* _currentFd;

		std::unique_ptr<Fd> _wakeupFd;
		std::unique_ptr<Poller> _poller;

		std::mutex _mutex;
		std::vector<Functor> _functors;
	};
}


#endif //LIBSAF_EVENTLOOP_H
