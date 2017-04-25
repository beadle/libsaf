//
// Created by beadle on 4/14/17.
//

#ifndef EXAMPLE_OBJECTBINDER_H
#define EXAMPLE_OBJECTBINDER_H

#include <functional>
#include <memory>
#include <iostream>


namespace saf
{
	template <class T>
	void PointerDeleter(void* pointer)
	{
		delete static_cast<T*>(pointer);
	}

	class SharedBinder
	{
	public:
		typedef std::function<void(void*)> Deleter;

	public:
		SharedBinder() :
			_pointer(nullptr),
			_deleter(nullptr)
		{

		}

		~SharedBinder()
		{
			unbindObject();
		}

		template <class T>
		void bindObject(const std::shared_ptr<T>& pointer)
		{
			_pointer = new std::shared_ptr<T>(pointer);
			_deleter = &PointerDeleter<std::shared_ptr<T> >;
		}

		template <class T>
		const std::shared_ptr<T>& getBindObject()
		{
			return *(static_cast<std::shared_ptr<T>* >(_pointer));
		}

		void unbindObject()
		{
			if (_pointer)
			{
				_deleter(_pointer);
				_pointer = nullptr;
			}
		}

	private:
		void* _pointer;
		Deleter _deleter;
	};


	class WeakBinder
	{
	public:
		typedef std::function<void(void*)> Deleter;

	public:
		SharedBinder() :
				_pointer(nullptr),
				_deleter(nullptr)
		{

		}

		~SharedBinder()
		{
			unbindObject();
		}

		template <class T>
		void bindShared(const std::shared_ptr<T>& pointer)
		{
			_pointer = new std::shared_ptr<T>(pointer);
			_deleter = &PointerDeleter<std::shared_ptr<T> >;
		}

		template <class T>
		void bindWeak(const std::shared_ptr<T>& pointer)
		{
			_pointer = new std::weak_ptr<T>(pointer);
			_deleter = &PointerDeleter<std::weak_ptr<T> >;
		}

		template <class T>
		void bindRaw(T* pointer)
		{
			_pointer = pointer;
			_deleter = &PointerDeleter<T >;
		}

		template <class T>
		std::shared_ptr<T> objectFromWeak()
		{
			return (static_cast<std::weak_ptr<T>*>(_pointer))->lock();
		}

		template <class T>
		const std::shared_ptr& objectFromShared()
		{
			return *(static_cast<std::shared_ptr<T>*>(_pointer));
		}

		template <class T>
		T* objectFromRaw()
		{
			return static_cast<T*>(_pointer);
		}

		void unbindObject()
		{
			if (_pointer)
			{
				_deleter(_pointer);
				_pointer = nullptr;
			}
		}

	private:
		void* _pointer;
		Deleter _deleter;
	};

}

#endif //EXAMPLE_OBJECTBINDER_H
