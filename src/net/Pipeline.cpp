//
// Created by beadle on 5/3/17.
//

#include "Pipeline.h"
#include "Handler.h"
#include "Buffer.h"


namespace saf
{
	void Pipeline::addFirst(HandlerPtr handler)
	{
		if (_handlers.empty())
		{
			addLast(handler);
		}
		else
		{
			_handlers.insert(_handlers.begin(), handler);
		}
	}

	void Pipeline::addLast(HandlerPtr handler)
	{
		_handlers.emplace_back(handler);
	}

	void Pipeline::process(Buffer *buffer)
	{
		BufferData* data = BufferData::create(buffer, buffer->readableBytes());
		std::shared_ptr<HandlerData> holder(data);

		for (auto handler : _handlers)
		{
			holder.reset(handler->process(this, holder.get()));
			if (holder->getStatus() != HandlerStatus::SUCCESS)
			{
				break;
			}
		}
	}
}