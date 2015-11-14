#ifndef TESTS_UTILS_H_
#define TESTS_UTILS_H_

#include <stdbool.h>
#include <stdio.h>

#define DEBUG_ON

#ifdef DEBUG_ON
#define ERROR(format, args...) fprintf(stderr, (format), ## args)

#define DEBUG_PRINT(format, args...) printf((format), ## args)
/**
 * Evaluates b and continues if b is true.
 * If b is false, ends the test by returning false and prints a detailed
 * message about the failure.
 */
#define ASSERT(b) do { \
    if (!(b)) { \
        fprintf(stderr, "\nAssertion failed at %s:%d %s \n",__FILE__,__LINE__,#b); \
        exit(1); \
    } \
} while (0)


#else /* not defined DEBUG_ON */
#define ERROR(format, args...)
#define DEBUG_PRINT(format, args...)
#define ASSERT(b)
#endif /* DEBUG_ON */

#endif /* TESTS_UTILS_H_ */