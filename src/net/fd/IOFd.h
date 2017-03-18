//
// Created by beadle on 3/15/17.
//

#ifndef LIBSAF_WATCHER_H
#define LIBSAF_WATCHER_H

#include "../Fd.h"
#include <functional>

namespace saf
{
	class Poller;
	class EventLoop;

	class IOFd : public Fd
	{
	public:
		typedef std::function<void()> EventCallback;
		enum class Status { NEW, ADDED, DELETED, };

		static const int kReadEvent;
		static const int kWriteEvent;

	public:
		void setReadCallback(EventCallback&& cb) { _readCallback = std::move(cb); }
		void setWriteCallback(EventCallback&& cb) { _writeCallback = std::move(cb); }
		void setCloseCallback(EventCallback&& cb) { _closeCallback = std::move(cb); }
		void setErrorCallback(EventCallback&& cb) { _errorCallback = std::move(cb); }

		void enableRead();
		void disableRead();
		void enableWrite();
		void disableWrite();
		void disableAll();

		void handleEvent();

		bool isReading() const { return bool(_events & kReadEvent); }
		bool isWriting() const { return bool(_events & kWriteEvent); }
		bool isNoneEvent() const { return !_events; }

		int getEvents() const { return _events; }

	public:  // exposed to Poller only
		IOFd(EventLoop *looper, int fd);
		~IOFd();

		Status getStatus() const { return _status; }
		EventLoop* getLooper() const { return _looper; }
		void setStatus(Status status) { _status = status; }
		void setREvent(int event) { _revents = event; }

	protected:
		void update();

	private:
		bool _handling;
		int _events;
		int _revents;
		Status _status;
		EventLoop* _looper;

		EventCallback _readCallback;
		EventCallback _writeCallback;
		EventCallback _closeCallback;
		EventCallback _errorCallback;
	};
}


#endif //LIBSAF_WATCHER_H
