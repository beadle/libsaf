//
// Created by beadle on 3/29/17.
//

#include <unistd.h>
#include <iostream>
#include "core.h"

using namespace saf;


class Master;

class Session
{
public:
	Session(EventLoop* loop, Master* master, const InetAddress& serverAddr);

	void connect()
	{
		_client->connect(_addr);
	}

	void disconnect()
	{
		_client->disconnect();
	}

	bool isConnected() const { return _client->isConnected(); }

	TcpClient* getClient() { return _client.get(); }

	int64_t getReadBytes() const { return _readBytes; }
	int64_t getWriteBytes() const { return _writeBytes; }
	int64_t getReadMessages() const { return _readMessages; }

private:
	std::shared_ptr<TcpClient> _client;
	Master* _master;
	InetAddress _addr;

	int64_t _readBytes;
	int64_t _writeBytes;
	int64_t _readMessages;
};


class Master
{
public:
	Master(EventLoop* loop,
		   const InetAddress& serverAddr,
		   int blockSize,
		   int sessionCount,
		   int timeout,
		   int threadCount)
		: _loop(loop),
		  _threadPool(loop),
		  _sessionCount(sessionCount),
		  _timeout(timeout),
		  _numConnected(0)
	{
		for (int i=0; i<blockSize; ++i)
			_message.push_back(static_cast<char>(i % 128));

		_threadPool.start(static_cast<size_t>(threadCount));

		_loop->addTimer(timeout, [this]()
		{
			handleTimeout();
		});

		for (int i=0; i<_sessionCount; ++i)
		{
			_sessions.emplace_back(new Session(_threadPool.getNextLoop(), this, serverAddr));
			_sessions.back()->connect();
		}
	}

	~Master()
	{

	}

	const std::string& getMessage() const { return _message; }

protected:
	void handleTimeout()
	{
		LOG_WARN("Master::handleTimeout")
		for(auto session : _sessions)
		{
			session->disconnect();
		}
	}

	void onConnected()
	{
		if (++_numConnected == _sessionCount)
		{
			LOG_WARN("all connected");
		}
	}

	void onDisconnected(const ConnectionPtr& conn)
	{
		if (--_numConnected == 0)
		{
			int64_t totalBytesRead = 0;
			int64_t totalMessagesRead = 0;
			for (auto it = _sessions.begin(); it != _sessions.end(); ++it)
			{
				totalBytesRead += (*it)->getReadBytes();
				totalMessagesRead += (*it)->getReadMessages();
			}

			LOG_WARN("total read bytes: %ld", totalBytesRead);
			LOG_WARN("total read messages: %ld", totalMessagesRead);

			LOG_WARN("average message size: %f", static_cast<double>(totalBytesRead) / static_cast<double>(totalMessagesRead))
			LOG_WARN("MiB/s throughput: %f", static_cast<double>(totalBytesRead) / (_timeout * 1024 * 1024));

			_loop->queueInLoop([this]()
			{
				_threadPool.stop();
				_loop->stop();
			});
		}
	}

	friend class Session;

private:
	EventLoop* _loop;
	EventLoopCluster _threadPool;
	int _sessionCount;
	int _timeout;
	std::vector<std::shared_ptr<Session> > _sessions;
	std::string _message;
	std::atomic_int _numConnected;

};


Session::Session(EventLoop* loop, Master* master, const InetAddress& serverAddr) :
		_client(new TcpClient(loop)),
		_master(master),
		_addr(serverAddr),
		_readBytes(0),
		_writeBytes(0),
		_readMessages(0)
{
	_client->setConnectChangeCallback([this](const ConnectionPtr& conn)
	{
		if (conn->isConnected())
		{
			conn->setTcpNoDelay(true);
			conn->send(_master->getMessage());
			_master->onConnected();
		}
		else
		{
			_master->onDisconnected(conn);
		}
	});
	_client->setRecvMessageCallback([this](const ConnectionPtr& conn, Buffer* buffer)
	{
		++_readMessages;
		_readBytes += buffer->readableBytes();
		_writeBytes += buffer->readableBytes();
		conn->send(buffer->peek(), buffer->readableBytes());
		buffer->retrieveAll();
	});
}


int main(int argc, char* argv[])
{
	if (argc != 7)
	{
		fprintf(stderr, "Usage: client <host_ip> <port> <threads> <blocksize> ");
		fprintf(stderr, "<sessions> <time>\n");
		return 0;
	}

	INIT_LOGGER("log/client.log", LogLevel::WARN)

	const char* ip = argv[1];
	uint16_t port = static_cast<uint16_t>(atoi(argv[2]));
	int threadCount = atoi(argv[3]);
	int blockSize = atoi(argv[4]);
	int sessionCount = atoi(argv[5]);
	int timeout = atoi(argv[6]);

	LOG_INFO("pid = %d, tid = %d, threadCount = %d, sessionCount = %d, timeout = %d",
			 ::getpid(), CurrentThread::tid(), threadCount, sessionCount, timeout);

	EventLoop loop;
	InetAddress serverAddr(ip, port);

	Master master(&loop, serverAddr, blockSize, sessionCount, timeout, threadCount);

	loop.start();
	return 0;
}