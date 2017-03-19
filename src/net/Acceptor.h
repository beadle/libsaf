//
// Created by beadle on 3/14/17.
//

#ifndef LIBSAF_ACCEPTOR_H
#define LIBSAF_ACCEPTOR_H

#include <functional>

namespace saf
{

	class Acceptor
	{
	public:
		typedef std::function<void(int)> NewConnectionCallback;
	};
}


#endif //LIBSAF_ACCEPTOR_H
