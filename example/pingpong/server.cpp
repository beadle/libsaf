//
// Created by beadle on 3/29/17.
//

#include <iostream>
#include <unistd.h>
#include "core.h"

using namespace saf;


int main(int argc, char* argv[])
{
	if (argc < 4)
	{
		fprintf(stderr, "Usage: server <address> <port> <threads>\n");
		return 1;
	}

	INIT_LOGGER("log/server.log", LogLevel::WARN)
	LOG_WARN("pid = %d, tid = %d", ::getpid(), CurrentThread::tid())
	std::cout << "pid = " << ::getpid() << " tid = " << CurrentThread::tid();

	const char* ip = argv[1];
	uint16_t port = static_cast<uint16_t>(atoi(argv[2]));
	InetAddress listenAddr(ip, port);
	int threadCount = atoi(argv[3]);

	EventLoop loop;

	TcpServer server(&loop);
	server.setRecvMessageCallback([&server](const ConnectionPtr& conn, Buffer* buffer)
	{
		conn->send(buffer->retrieveAllAsNetString());
	});
	server.setConnectChangeCallback([&server](const ConnectionPtr& conn)
	{
		conn->setTcpNoDelay(true);
	});
	server.start(listenAddr, static_cast<size_t>(threadCount));

	loop.start();
}