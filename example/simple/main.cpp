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

void test_timer(saf::EventLoop* loop)
{
	for (int i=0; i<50; ++i)
	{
		float delay = 2.0f + (float)((double) rand() / (RAND_MAX)) * 8.0f;
		loop->addTimer(delay, [delay](){
			std::cout << "once timer(" << delay << ")" << std::endl;
		});
	}

	for (int i=0; i<5; ++i)
	{
		loop->addTimer(4.0, [loop](){
			int fd = 1 + (int)(((double) rand() / (RAND_MAX)) * 40.0f);
			loop->cancelTimer(fd);
			std::cout << "cancel timer(" << fd << ")" << std::endl;
		});
	}

	for (int i=0; i<5; ++i)
	{
		loop->addTimer(5.0, [loop](){
			float delay = 2.0f + (float)((double) rand() / (RAND_MAX)) * 3.0f;
			loop->addTimer(delay, [delay](){
				std::cout << "inner add timer(" << delay + 5.0 << ")" << std::endl;
			});
		});
	}

	int interval = 3;
	loop->addTimer(interval, [interval](){
		std::cout << "repeated timer(" << interval << ")" << std::endl;
	}, true);

}

void thread1_func(saf::EventLoop* loop)
{
	loop->runInLoop([loop](){
		std::cout << "Thread1 func" << std::endl;
	});
}

void thread2_func(saf::EventLoop* loop)
{
	test_timer(loop);
}


class A
{
public:
	A() {
		std::cout << this << __FUNCTION__ << "(" << __LINE__ << ")" << std::endl;
	}
	A(const A& other) {
		std::cout << this << __FUNCTION__ << "(" << __LINE__ << ")" << std::endl;
	}
	A(A&& other) {
		std::cout << this << __FUNCTION__ << "(" << __LINE__ << ")" << std::endl;
	}
	void operator=(const A& other) {
		std::cout << this << __FUNCTION__ << "(" << __LINE__ << ")" << std::endl;
	}
	void operator=(A&& other) {
		std::cout << this << __FUNCTION__ << "(" << __LINE__ << ")" << std::endl;
	}
	~A() {
		std::cout << this << __FUNCTION__ << "(" << __LINE__ << ")" << std::endl;
	}
};

void func(A&& a) {
	A store;
	store = a;
	std::cout << "in function:" << &a << std::endl;
}


int main()
{
	INIT_LOGGER;

	EventLoop loop;

	Server server(&loop, InetAddress(5000), NetProtocal::UDP);
	server.start(1);

	loop.addTimer(5.0, [&server](){
		server.stop();
	});

	Client client(&loop, InetAddress("127.0.0.1", 5000), NetProtocal::TCP, 3.0f);
	client.setRecvMessageCallback([](const ConnectionPtr& conn, Buffer* buffer){
		LOG_INFO("Client RecvMessageCallback: %s", buffer->retrieveAllAsString().c_str());
	});

	loop.addTimer(1.0, [&client](){
		client.connect();
	});

	loop.addTimer(3.0, [&client](){
		static int counter = 'A';
		auto connection = client.getConnection();
		if (connection)
			connection->send(std::string("Hello World: ") + char(counter++));
	}, true);

	loop.start();

	return 0;
}