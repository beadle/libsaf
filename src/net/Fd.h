//
// Created by beadle on 3/15/17.
//

#ifndef LIBSAF_WATCHER_H
#define LIBSAF_WATCHER_H

#include <functional>

namespace saf
{
	class Poller;
	class EventLoop;

	class Fd
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

		inline bool isReading() const { return bool(_events & kReadEvent); }
		inline bool isWriting() const { return bool(_events & kWriteEvent); }
		inline bool isNoneEvent() const { return !_events; }

		inline int getFd() const { return _fd; }
		inline int getEvents() const { return _events; }

	public:  // exposed to Poller only
		Fd(EventLoop *looper, int fd);
		~Fd();

		inline Status getStatus() const { return _status; }
		inline EventLoop* getLooper() const { return _looper; }
		inline void setStatus(Status status) { _status = status; }
		inline void setREvent(int event) { _revents = event; }

	protected:
		void update();

	private:
		bool _handling;
		int _fd;
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
