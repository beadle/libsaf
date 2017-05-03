//
// Created by beadle on 5/3/17.
//

#include "UnpackHandler.h"
#include "net/Buffer.h"


namespace saf
{

	HandlerData* UnpackHandler::process(Pipeline* pipeline, HandlerData* data)
	{
		auto* context = data->as<BufferData>();
		Buffer* buffer = context->buffer;

		if (buffer->readableBytes() > HeaderBits)
		{
			auto size = buffer->peekInt32();
			if (size > 0 && buffer->readableBytes() >= size + HeaderBits)
			{
				buffer->retrieve(HeaderBits);

				auto package = BitsData::create(buffer->peek(), static_cast<size_t>(size));
				buffer->retrieve(static_cast<size_t>(size));
				return package;
			}
		}

		data->setStatus(HandlerStatus::FAILED);
		return data;
	}
}