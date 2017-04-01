//
// Created by beadle on 3/27/17.
//

#include "Connection.h"
#include "EventLoop.h"

#include "base/TimeUtils.h"


namespace saf
{

	Connection::Connection(EventLoop *loop, const std::string& key, const InetAddress &addr) :
		_loop(loop),
		_index(key),
		_addr(addr),
		_activedTime(time::timestamp()),
		_recvMessageCallback(nullptr),
		_writeCompleteCallback(nullptr),
		_connectChangeCallback(nullptr),
		_closeCallback(nullptr)
	{

	}

	Connection::~Connection()
	{

	}

	void Connection::send(NetString data)
	{
		send(data.getData(), data.getLength());
	}

	void Connection::send(const char *data, size_t length)
	{
		if (_loop->isInLoopThread())
		{
			sendInLoop(data, length);
		}
		else
		{
			std::shared_ptr<std::string> copied(new std::string(data, length));
			_loop->queueInLoop([this, copied]()
			{
				sendInLoop(copied->c_str(), copied->length());
			});
		}
	}
}