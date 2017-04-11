//
// Created by beadle on 4/11/17.
//

#ifndef EXAMPLE_NONCOPYABLE_H
#define EXAMPLE_NONCOPYABLE_H


class Noncopyable
{
protected:
	Noncopyable() {}
	~Noncopyable() {}

private:  // emphasize the following members are private
	Noncopyable( const Noncopyable& );
	const Noncopyable& operator=( const Noncopyable& );
};


#endif //EXAMPLE_NONCOPYABLE_H
