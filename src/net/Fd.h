//
// Created by beadle on 3/18/17.
//

#ifndef EXAMPLE_FD_H
#define EXAMPLE_FD_H


namespace saf
{

	class Fd
	{
	public:
		Fd(int fd) : _fd(fd) {}
		virtual ~Fd() {}

		int getFd() const { return _fd; }

	private:
		int _fd;
	};

}


#endif //EXAMPLE_FD_H
