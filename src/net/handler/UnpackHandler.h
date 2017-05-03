//
// Created by beadle on 5/3/17.
//

#ifndef EXAMPLE_UNPACKHANDLER_H
#define EXAMPLE_UNPACKHANDLER_H

#include "net/Handler.h"


namespace saf
{

	class UnpackHandler : public Handler
	{
	public:
		static const size_t HeaderBits = 32;

	public:
		HandlerData* process(Pipeline*, HandlerData*);

	};

}

#endif //EXAMPLE_UNPACKHANDLER_H
