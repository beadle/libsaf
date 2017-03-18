//
// Created by beadle on 3/18/17.
//

#ifndef EXAMPLE_TIMERQUEUE_H
#define EXAMPLE_TIMERQUEUE_H

#include <vector>
#include <unordered_map>
#include <bits/unique_ptr.h>

#include "fd/Timer.h"


namespace saf
{
	class EventLoop;

	class TimerQueue
	{
	protected:
		typedef std::unique_ptr<Timer> TimerPtr;

		TimerQueue(EventLoop* loop);

		int addTimer(float delay, TimerCallback&& callback, bool repeated=false);
		void cancelTimer(int fd);

		bool checkTimers() const;
		void popTimer();

		friend class EventLoop;

	protected:
		void heapInsert(Timer* timer);
		void heapDown(int pos);
		void heapUp(int pos);
		bool heapDelete(int pos);
		Timer* heapPop();
		inline void heapSwap(int posA, int posB);

	private:
		int _counter;
		EventLoop* _loop;

		std::vector<Timer*> _heap;
		std::unordered_map<int, TimerPtr> _timers;
	};
}

#endif //EXAMPLE_TIMERQUEUE_H
