//
// Created by beadle on 3/17/17.
//

#ifndef EXAMPLE_CURRENTTHREAD_H
#define EXAMPLE_CURRENTTHREAD_H

namespace saf
{
	namespace CurrentThread
	{
		extern __thread int t_cachedTid;

		void cacheTid();

		inline int tid()
		{
			if (__builtin_expect(t_cachedTid == 0, 0))
			{
				cacheTid();
			}
			return t_cachedTid;
		}

		bool isMainThread();
	}
}

#endif //EXAMPLE_CURRENTTHREAD_H
