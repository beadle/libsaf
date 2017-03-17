//
// Created by beadle on 3/17/17.
//

#include "Thread.h"
#include "CurrentThread.h"

#include <unistd.h>
#include <sys/syscall.h>

namespace saf
{
	namespace CurrentThread
	{
		__thread int t_cachedTid = 0;

		void cacheTid()
		{
			if (t_cachedTid == 0)
			{
				t_cachedTid = static_cast<pid_t>(::syscall(SYS_gettid));
			}
		}

		bool isMainThread() {
			return tid() == ::getpid();
		}
	}

	std::atomic_int Thread::s_threadCount(0);

	Thread::Thread(ThreadFunc&& threadFunc, const std::string& name) :
		_threadFunc(threadFunc),
		_name(name)
	{
		++s_threadCount;
	}

	Thread::~Thread()
	{
	}

	void Thread::start()
	{
		_thread.reset(new std::thread(_threadFunc));
	}

	void Thread::join()
	{
		_thread->join();
	}

}