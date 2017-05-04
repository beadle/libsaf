//
// Created by beadle on 5/3/17.
//

#ifndef EXAMPLE_HANDLER_H
#define EXAMPLE_HANDLER_H

#include <memory>


namespace saf
{
	class Buffer;
	class Pipeline;

	enum class HandlerStatus
	{
		SUCCESS,
		FAILED,
	};


	class HandlerData
	{
	public:

		template <class T>
		T* as()
		{
			return static_cast<T*>(this);
		}

		HandlerStatus getStatus() const { return _status; }
		void setStatus(HandlerStatus status) { _status = status; }

	protected:
		HandlerData() :
				_status(HandlerStatus::SUCCESS)
		{
		}

	private:
		HandlerStatus _status;
	};


	class BufferData : public HandlerData
	{
	public:
		static BufferData* create(Buffer* buffer, size_t length)
		{
			auto data = new BufferData;
			data->buffer = buffer;
			data->length = length;
			return data;
		}

		Buffer* buffer;
		size_t length;
	};


	class BitsData : public HandlerData
	{
	public:
		static BitsData* create(const void* buffer, size_t length)
		{
			auto data = new BitsData;
			data->data = buffer;
			data->length = length;
			return data;
		}

		const void* data;
		size_t length;
	};


	class Handler
	{
	public:
		virtual HandlerData* process(Pipeline*, HandlerData*) = 0;
	};

}

#endif //EXAMPLE_HANDLER_H
