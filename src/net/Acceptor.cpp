//
// Created by beadle on 3/28/17.
//

#include "Acceptor.h"
#include "fd/Socket.h"


namespace saf
{
	Acceptor::Acceptor(EventLoop *loop) :
		_loop(loop),
		_listening(false),
		_socket(nullptr)
	{

	}

	Acceptor::~Acceptor()
	{

	}
}