//
// Created by beadle on 5/3/17.
//

#ifndef EXAMPLE_PIPELINE_H
#define EXAMPLE_PIPELINE_H

#include <vector>
#include <memory>


namespace saf
{
	class Buffer;
	class Handler;

	class Pipeline
	{
	public:
		typedef std::shared_ptr<Handler> HandlerPtr;

	public:
		void addLast(HandlerPtr handler);
		void addFirst(HandlerPtr handler);

		void process(Buffer* buffer);

	private:
		std::vector<HandlerPtr> _handlers;
	};

}

#endif //EXAMPLE_PIPELINE_H
