//
// Created by beadle on 3/23/17.
//

#ifndef EXAMPLE_TYPES_H
#define EXAMPLE_TYPES_H

#include <memory>
#include <functional>


namespace saf
{
	class IOFd;
	class Buffer;
	class Acceptor;
	class Connection;

	typedef std::shared_ptr<Connection> ConnectionPtr;

	typedef std::function<void(const ConnectionPtr&, Buffer*)> RecvMessageCallback;
	typedef std::function<void(const ConnectionPtr&)> ConnectChangeCallback;
	typedef std::function<void(const ConnectionPtr&)> WriteCompleteCallback;
	typedef std::function<void(const ConnectionPtr&)> CloseCallback;

	enum class NetProtocal { TCP, UDP, KCP, };

	class IOFdObserver
	{
	public:
		virtual void onReadInIOFd(IOFd*) {};
		virtual void onWriteInIOFd(IOFd*) {};
		virtual void onErrorInIOFd(IOFd*) {};
		virtual void onCloseInIOFd(IOFd*) {};
	};

}


#endif //EXAMPLE_TYPES_H
