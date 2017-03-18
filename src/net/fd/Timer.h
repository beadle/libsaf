//
// Created by beadle on 3/16/17.
//

#ifndef LIBSAF_TIMER_H
#define LIBSAF_TIMER_H


#include "../Fd.h"
#include <functional>

namespace saf
{
	typedef std::function<void()> TimerCallback;

	class Timer : public Fd
	{
	protected:
		Timer(int fd, long interval, TimerCallback&& callback);

		void handleTimeout();

		long getInterval() const { return _interval; }

		void setTimestamp(const long& timestamp) { _timestamp = timestamp; }
		long getTimestamp() const { return _timestamp; }

		void setRepeated(bool repeated) { _repeated = repeated; }
		bool getRepeated() const { return _repeated; }

		int getHeapIndex() const { return _heapIndex; }
		void setHeapIndex(int index) { _heapIndex = index; }

		friend class TimerQueue;

	private:
		bool _repeated;
		int _heapIndex;
		long _interval;
		long _timestamp;

		TimerCallback _callback;
	};

}

#endif //LIBSAF_TIMER_H
