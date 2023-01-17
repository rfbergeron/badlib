#ifndef __BADLIB_H__
#define __BADLIB_H__

#ifdef UNIT_TESTING
extern void _test_free(void* const ptr, const char* file, const int line);
#define DESTROY_DATA(FN, DATA) \
  ((FN) == free ? _test_free((DATA), __FILE__, __LINE__) : (FN)(DATA))
#else
#define DESTROY_DATA(FN, DATA) ((FN)(DATA))
#endif

typedef enum badlib_error {
  BLIB_SUCCESS,
  BLIB_ALLOC_FAIL,
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
