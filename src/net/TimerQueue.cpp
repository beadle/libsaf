//
// Created by beadle on 3/18/17.
//
#include <chrono>

#include "TimerQueue.h"
#include "EventLoop.h"
#include "base/TimeUtils.h"


namespace saf
{
	static const int FirstItem = 1;

	TimerQueue::TimerQueue(EventLoop *loop) :
		_counter(0),
		_loop(loop),
		_heap(FirstItem)
	{
	}

	int TimerQueue::addTimer(float delay, TimerCallback &&callback, bool repeated)
	{
		Timer* ptr = new Timer(++_counter, (long)(delay * 1000), std::move(callback));
		_loop->runInLoop([=](){
			TimerPtr timer(ptr);
			timer->setRepeated(repeated);
			_timers[timer->getFd()] = std::move(timer);
			heapInsert(ptr);
		});
		return ptr->getFd();
	}

	void TimerQueue::cancelTimer(int fd)
	{
		_loop->runInLoop([=](){
			auto it = _timers.find(fd);
			if (it != _timers.end())
			{
				int pos = it->second->getHeapIndex();
				heapDelete(pos);
				_timers.erase(fd);
			}
		});
	}

	bool TimerQueue::checkTimers() const
	{
		if (_heap.size() <= FirstItem)
			return false;

		return _heap[FirstItem]->getTimestamp() < time::timestamp();
	}

	void TimerQueue::popTimer()
	{
		auto timer = heapPop();
		if (!timer->getRepeated())
		{
			TimerPtr ptr = std::move(_timers[timer->getFd()]);
			_timers.erase(timer->getFd());
			ptr->handleTimeout();
		}
		else
		{
			timer->setTimestamp(time::timestamp() + timer->getInterval());
			heapInsert(timer);
			timer->handleTimeout();
		}
	}

	void TimerQueue::heapInsert(Timer *timer)
	{
		int index = static_cast<int>(_heap.size());
		_heap.push_back(timer);
		timer->setHeapIndex(index);
		heapUp(index);
	}

	Timer* TimerQueue::heapPop()
	{
		if (_heap.size() <= FirstItem)
			return nullptr;

		heapSwap(FirstItem, (int)_heap.size() - 1);
		Timer* timer = _heap.back();
		_heap.pop_back();
		heapDown(FirstItem);
		return timer;
	}

	bool TimerQueue::heapDelete(int pos)
	{
		if (pos <= 0 || pos >= _heap.size())
			return false;

		heapSwap(pos, (int)_heap.size() - 1);
		_heap.pop_back();
		heapDown(pos);
		return true;
	}

	void TimerQueue::heapDown(int pos)
	{
		while (true)
		{
			int target = pos;
			int left = pos << 1;
			int right = left | 1;

			if (left >= _heap.size())
				return;

			if (_heap[left]->getTimestamp() < _heap[target]->getTimestamp())
				target = left;

			if (right < _heap.size())
			{
				if (_heap[right]->getTimestamp() < _heap[target]->getTimestamp())
					target = right;
			}

			if (pos != target)
			{
				heapSwap(pos, target);
				pos = target;
			}
			else return;
		}

	}

	void TimerQueue::heapUp(int pos)
	{
		while (pos > 1)
		{
			int parent = pos >> 1;
			if (_heap[parent]->getTimestamp() > _heap[pos]->getTimestamp())
			{
				heapSwap(pos, parent);
				pos = parent;
			}
			else return;
		}
	}

	void TimerQueue::heapSwap(int posA, int posB)
	{
		std::swap(_heap[posA], _heap[posB]);
		_heap[posA]->setHeapIndex(posA);
		_heap[posB]->setHeapIndex(posB);
	}


}