This will be a simple generic data structures library written in C for use with
my other garbage programs.

# Structures to implement:
- Doubly Linked List, which will also serve as a stack and deque
- Resizeable array
- Heap
- Map
- Set
- Weight-Balanced Binary Tree, which will serve as a binary tree and ordered map/set

# Actual TODO
- uniform return values for errors/other circumstances (return `NULL` or
  `-1` or similar and set an error field in the data structure if something
  went wrong)
- have makefile compile murmur in a more normal way, via recursive call
- proper unit testing for map and arraylist
- macroify structures
- change naming of variables referring to current number of elements and
  maxmimum number of elements in the structure to length and capacity, like
  golang does
- remove trivial, unnecessary functions, like `X_size`

# Variadic versions of foreach functions
It may be convenient to have variadic versions of each of the foreach functions,
which take a variadic function as an argument instead of one with a fixed single
argument.

This way, an arbitrary number of data structures can be composed together using
a function, such as adding/multiplying/etc. elements at the same index.

However, this would make the foreach function less flexible. Functions would
have to be purpose-written to be supplied as an argument to the foreach
functions, instead of the simpler current implementation.

# Pointer arguments vs. value arguments and allocation of space
Requiring the user to allocate space makes it clearer who is responsible for
freeing it and allows stack-stored values to be initialized with the same
functions, but makes using the structures more long-winded.

It may be better to make the storage space argument optional and have the
function perform the allocation if `NULL` is passed. Errors would need to be
communicated with an additional output argument. Or, the same argument could be
used by creating a union of an integer and pointer, along with a flag. If I
wanted to be 'clever', I could even use a single argument by setting the flag
argument to 1. Alignment requirements would make the least significant bit of
pointers 0 on most architectures, so if set the pointer is either invalid or
the caller passed a flag.

When the structure is destroyed, the destroyer function could have a flag
argument indicating whether or not to free the data structure. Or, the flag
could be stored in the data structure, set when it is initialized, and checked
when it is destroyed.

# Covert to macros
The library could be made even more general, if slightly more cumbersome to use,
by using macros to allow the base type of the structures to be changed, rather
than just using `void*` for everything. After using the macro to generate the
functions and structures, it would be equivalent. The current filenames would
remain the same, but instead of containing code, they would call a handful of
macros to handle all of the setup. These macros would be defined in a separate
file or set of files. Each type of structure would have one set of macros, one
for the generation of headers, which would contain function prototypes, struct
definitions, and global variables, while the other would contain function
definitions for the creation of source files.

## Macro structure
Macros would take as arguments:
- The types of the values that the structures would be storing (values for all,
  and additionally keys for maps.
- The typedef'd and bare names of the data structure. The bare name would be
  used for both the struct definition and the function names.

# Linked List

## Structure Definition
Fields will include:
- pointer to this list's anchor
- length of list
- the status of the last operation that was performed on the list

## Error codes
These error codes will be returned by linked list operations, and will be
present in the status field of the linked list.

### SUCCESS
### LIST_UNINITIALIZED
### LIST_NULL
### LIST_NOT_FOUND
### LIST_EMPTY
