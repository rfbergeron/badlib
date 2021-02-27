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
