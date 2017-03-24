//
// Created by beadle on 3/21/17.
//

#ifndef EXAMPLE_NETSTRING_H
#define EXAMPLE_NETSTRING_H

#include <string>
#include <string.h>


namespace saf
{

	class NetString
	{
	public:
		NetString() : _data(nullptr), _length(0) {}
		NetString(const char* data) : _data(data), _length(strlen(data)) {}
		NetString(const char* data, size_t len) : _data(data), _length(len) {}
		NetString(const std::string& data) : _data(data.c_str()), _length(data.size()) {}

		std::string toString() const { return std::string(_data, _length); }

		const char* getData() const { return _data; }
		size_t getLength() const { return _length; }

	private:
		const char* _data;
		size_t _length;
	};

}

#endif //EXAMPLE_NETSTRING_H
