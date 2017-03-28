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

	public: /// Thread-Safed Methods
		void setReadCallback(const EventCallback& cb) { _readCallback = std::move(cb); }
		void setWriteCallback(const EventCallback& cb) { _writeCallback = std::move(cb); }
		void setCloseCallback(const EventCallback& cb) { _closeCallback = std::move(cb); }
		void setErrorCallback(const EventCallback& cb) { _errorCallback = std::move(cb); }

		bool isReading() const { return bool(_events & kReadEvent); }
		bool isWriting() const { return bool(_events & kWriteEvent); }
		bool isNoneEvent() const { return !_events; }

	public:  /// Looper Thread Methods
		void enableReadInLoop();
		void disableReadInLoop();
		void enableWriteInLoop();
		void disableWriteInLoop();
		void disableAllInLoop();

		void attachInLoop(EventLoop *looper);
		bool detachInLoop();

	public:  /// Exposed To Pollers Only
		IOFd(int fd);
		virtual ~IOFd();

		int getEvents() const { return _events; }
		Status getStatus() const { return _status; }
		EventLoop* getLooper() const { return _looper; }

		void setStatus(Status status) { _status = status; }
		void setREvent(int event) { _revents = event; }

		virtual void handleEvent();

	protected:
		static const int kReadEvent;
		static const int kWriteEvent;

		void update();

	protected:
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
