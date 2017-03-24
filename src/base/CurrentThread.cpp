//
// Created by beadle on 3/23/17.
//
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

		bool isMainThread()
		{
			return tid() == ::getpid();
		}
	}
}