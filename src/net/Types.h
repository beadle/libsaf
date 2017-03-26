//
// Created by beadle on 3/23/17.
//

#ifndef EXAMPLE_TYPES_H
#define EXAMPLE_TYPES_H

#include <memory>
#include <functional>


namespace saf
{
	class Buffer;
	class Connection;

	typedef std::shared_ptr<Connection> ConnectionPtr;

	typedef std::function<void(const ConnectionPtr&, Buffer*)> RecvMessageCallback;
	typedef std::function<void(const ConnectionPtr&)> ConnectChangeCallback;
	typedef std::function<void(const ConnectionPtr&)> WriteCompleteCallback;
	typedef std::function<void(const ConnectionPtr&)> CloseCallback;


	class ConnectionObserver
	{
	public:
		virtual void onReceivedMessageInConnection(const ConnectionPtr&, Buffer*) = 0;
		virtual void onWriteCompletedInConnection(const ConnectionPtr&) = 0;
		virtual void onConnectChangedInConnection(const ConnectionPtr&) = 0;
		virtual void onClosedInConnection(const ConnectionPtr&) = 0;
	};

	enum class NetProtocal {
		TCP,
		UDP,
		KCP,
	};

}


#endif //EXAMPLE_TYPES_H
