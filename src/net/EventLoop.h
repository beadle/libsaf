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
	class IOFd;
	class Poller;
	class TimerQueue;

	class EventLoop
	{
	public:
		typedef std::function<void()> Functor;

	public:  // exposed to outers
		EventLoop();
		~EventLoop();

		void start();
		void stop();

		void runInLoop(Functor&& functor);
		void queueInLoop(Functor&& functor);

		void runInLoop(const Functor& functor);
		void queueInLoop(const Functor& functor);

		int addTimer(float delay, Functor&& callback, bool repeated=false);
		void cancelTimer(int fd);

		bool isInLoopThread() { return _threadId == CurrentThread::tid(); }
		void assertInLoopThread() { if (!isInLoopThread()) abortNotInLoopThread(); }

	public:  // exposed to IOFd only
		void updateFd(IOFd* fd);
		void removeFd(IOFd *fd);

	protected:
		void wakeup();
		void handleWakeupRead();
		void abortNotInLoopThread();

		void runFunctors();
		void runTimers();

	private:
		const pid_t _threadId;

		bool _quit;
		bool _looping;
		bool _handling;

		IOFd* _currentFd;

		std::unique_ptr<IOFd> _wakeupFd;
		std::unique_ptr<Poller> _poller;
		std::unique_ptr<TimerQueue> _timerQueue;

		std::mutex _mutex;
		std::vector<Functor> _functors;
	};
}


#endif //LIBSAF_EVENTLOOP_H
