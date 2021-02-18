#ifndef __BADLIB_H__
#define __BADLIB_H__
typedef enum badlib_error {
    BLIB_SUCCESS,
    BLIB_NULL_ARG,
    BLIB_UNINITIALIZED_ARG,
    BLIB_EMPTY,
    BLIB_NOT_FOUND,
    BLIB_OUT_OF_BOUNDS,
    BLIB_COMPARE_ERROR,
    BLIB_DESTROY_ERROR,
    BLIB_INVALID_SIZE
} BlibError;

typedef void (*BlibDestroyer)(void*);
typedef int (*BlibComparator)(void*,void*);
#endif
