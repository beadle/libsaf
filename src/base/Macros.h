//
// Created by beadle on 4/1/17.
//

#ifndef EXAMPLE_MACROS_H
#define EXAMPLE_MACROS_H

#define LIKELY(x) 		__builtin_expect(!!(x), 1)
#define UNLIKELY(x)		__builtin_expect(!!(x), 0)


#endif //EXAMPLE_MACROS_H
