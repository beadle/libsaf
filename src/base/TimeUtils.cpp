//
// Created by beadle on 3/18/17.
//

#include <chrono>
#include "TimeUtils.h"

namespace saf
{
	namespace time
	{
		long timestamp()
		{
			std::chrono::milliseconds ms = std::chrono::duration_cast< std::chrono::milliseconds >(
					std::chrono::system_clock::now().time_since_epoch()
			);
			return ms.count();
		}
	}
}