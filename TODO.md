This will be a simple generic data structures library written in C for use with
my other garbage programs.

# Structures to implement:
- (Doubly?) Linked list/Deque?
- Resizeable array
- Heap
- HashMap? (will most likely end up using farmhash/cityhash)

# Linked List

## Structure Definition
Fields will include:
- pointer to this list's anchor
- length of list

## Function Definitions

### `llist_init`
Takes a pointer to an uninitialized linked list as an argument. Allocates an
anchor for this list.

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

### `llist_insert`
Takes a pointer to a llist, a `void *`, and an index. Attempts to insert the
provided value at the provided index.

### `llist_delete`
Takes a pointer to a llist and an index. Attempts to delete the element in the
list at the given index.

### `llist_get`
Takes a pointer to a llist and an index. Attempts to return the element at the
given location in the list.

### `llist_clear`
Takes a pointer to a llist and an 

### `llist_empty`
Takes a pointer to a llist. Returns 1 if the list is empty and 0 otherwise.

### `llist_size`
Takes a pointer to a llist. Returns the size of the list.

### `llist_free`
Takes a pointer to a llist. Frees the list and any nodes associated with it.
Note that it only frees the nodes and the list itself, not the data stored
inside the nodes.

# Node
The node structure will be used to hold the elements of a Linked List. Since it
is not a standalone structure, there are no functions dedicated to operating on
nodes.

## Structure Definition
Fields will include:
- pointer to next node
- pointer to previous node
