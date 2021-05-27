This will be a simple generic data structures library written in C for use with
my other garbage programs.

# Structures to implement:
- (Doubly?) Linked list/Deque?
- Resizeable array
- Heap
- HashMap? (will most likely end up using farmhash/cityhash)

# Actual TODO
- return a more meaningful value on error than NULL so that the user can
  distinguish between when they are seeing a NULL value that they inserted
  and an error indicator
- have makefile compile murmur in a more normal way, via recursive call
- proper unit testing for map and arraylist
- maybe change format of library functions so that they always return an error
  code?
- macroify structures

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

## Function Definitions

### `llist_init`
Takes a pointer to an uninitialized linked list as an argument. Allocates an
anchor for this list.

### `llist_destroy`
Takes a pointer to an initialized linked list as an argument. Frees the memory
for the anchor and all elements still in the list. Note that it does not free
the `void *data` fields of the stored elements.

### `llist_push`
Takes a pointer to an llist and a `void *`. Prepends the element to the list.
Returns 0 on success and 1 otherwise.

### `llist_pop`
Takes a pointer to a llist. Removes the element at the beginning of the list
and returns it.

### `llist_peek`
Takes a pointer to a llist. Returns the element at the beginning of the list.

### `llist_push_back`
Takes a pointer to a llist and a `void *`. Appends the element to the end of the
llist. Returns 0 if the operation was successful and 1 otherwise.

### `llist_pop_back`
Takes a pointer to a llist. Removes the element at the end of the list and
returns it.

### `llist_peek_back`
Takes a pointer to a llist. Returns the element at the end of the list.

### `llist_clear`
Takes a pointer to a llist and an 

### `llist_empty`
Takes a pointer to a llist. Returns 1 if the list is empty and 0 otherwise.

### `llist_size`
Takes a pointer to a llist. Returns the size of the list.

# Node
The node structure will be used to hold the elements of a Linked List. Since it
is not a standalone structure, there are no functions dedicated to operating on
nodes.

## Structure Definition
Fields will include:
- pointer to next node
- pointer to previous node

# ArrayList

## Structure Definition
Fields:
- pointer to data
- currently allocated space
- user specified array size

## Function Definitions

# Map

## Architecture
The map's primary structure will be a dynamically allocated array of buckets.
Inserted values with different keys that map to the same slot will be appendend
to each other in a manner similar to a linked list. 

Since each node is going to be a linked list, it should have a structure similar
to the deque data structure, where there is an anchor node that does not store
anything but merely functions as a placeholder. The true head of the linked list
will be the node after this one. The node at the end of the list will point back
to the anchor; this means that, when the bucket is empty, the anchor will point to
itself.

Since the anchor node will be present until the map is destroyed, we have the
following guarantees:
1. every bucket in the chain will have a previous bucket
2. every bucket in the chain will have a succesor bucket
