#ifndef __BADLIB_H__
#define __BADLIB_H__
typedef enum badlib_error {
  BLIB_SUCCESS,
  BLIB_INVALID_STRUCT,
  BLIB_UNINITIALIZED_ARG,
  BLIB_EMPTY,
  BLIB_OUT_OF_BOUNDS,
  BLIB_COMPARE_ERROR,
  BLIB_DESTROY_ERROR,
  BLIB_INVALID_SIZE,
  W_BLIB_ELEMENT_REPLACED,
  W_BLIB_ARRAY_RESIZED,
  W_BLIB_NOT_FOUND
} BlibError;

typedef void (*BlibDestroyer)(void*);
typedef int (*BlibComparator)(void*, void*);
#endif
