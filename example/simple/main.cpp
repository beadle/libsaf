//
// Created by beadle on 3/17/17.
//
#include <random>
#include <thread>
#include <unistd.h>
#include <iostream>
#include <memory>

#include "core.h"

using namespace std;
using namespace saf;


int main()
{
	INIT_LOGGER("log/debug.log", LogLevel::DEBUG)

	EventLoop loop;

//	UdpServer udpServer(&loop);
//	udpServer.start(InetAddress("127.0.0.1", 5000), 1);
//	udpServer.setRecvMessageCallback([&udpServer](const ConnectionPtr& conn, Buffer* buffer)
//	{
//		conn->send(std::string("From UdpServer - "));
//		conn->send(buffer->retrieveAllAsNetString());
//	});

	std::shared_ptr<TcpServer> tcpServer(new TcpServer(&loop));
	tcpServer->start(InetAddress("127.0.0.1", 5000), 1);
	tcpServer->setRecvMessageCallback([&tcpServer](const ConnectionPtr& conn, Buffer* buffer)
	{
		conn->send(std::string("From TcpServer - "));
		conn->send(buffer->retrieveAllAsNetString());
	});
//	loop.addTimer(5.0, [&tcpServer](){
//		tcpServer.stop();
//	});

	std::shared_ptr<TcpClient> client(new TcpClient(&loop));
	client->setReconnectDelay(3.0);
	client->setRecvMessageCallback([](const ConnectionPtr& conn, Buffer* buffer)
	{
		LOG_INFO("TcpClient RecvMessageCallback: %s", buffer->retrieveAllAsString().c_str());
	});

	loop.addTimer(1.0, [&client]()
	{
		client->connect(InetAddress("127.0.0.1", 5000));
	});

	loop.addTimer(3.0, [&client]()
	{
		static int counter = 'A';
		auto connection = client->getConnection();
		if (connection)
			connection->send(std::string("Hello World: ") + char(counter++));
	}, true);

	loop.addTimer(5.0, [&client, &tcpServer, &loop](){
		client->disconnect();
		tcpServer->stop();
		loop.stop();
	});

	loop.start();

	return 0;
}