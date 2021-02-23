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

# how to do map buckets
Using an already implemented data structure, like the linked list, would be
somewhat easier. In this case, it would also be somewhat wasteful, since
it's a doubly linked list and we only need to search one way.

# thinky emoji
Maybe I should implement a more complicated hybrid data structure? It would
support both random access array and queue/deque operations. It would start out
as a contiguous array, but additional space would be allocated as non-contiguous
blocks, stored in a similar scheme to a linked list. Pushing and popping from
the back of the list would just insert the elements to the last empty array
index. On the other hand, pushing and popping from the

Each contiguous block in this generic list would need a substructure/sublist with
metadata that allows for the position of elements inside to be changed easily.
The metadata would need to contain:
- the allocated size of this contiguous block
- the first element of this contiguous block that is in use
- the number of indices following the first element that are in use
- the index of the first element of this block in the actual array
  NOTE: it may just be easier to store the offset or to not track this number at
  all since it could fairly easily be calculated by traversing the blocks
  in order and counting the number of indices in use in each block

For example, when an element is prepended to the array, a new block of data is
allocated. The element pushed onto the list is placed into the last space of
this block, which becomes the first used index of this block, and the first index
of the overall list. The number of used indices in this block becomes 1, and
every element in the original block allocated becomes shifted one index up.

Futher elements pushed to the list are placed into the preceeding slots in the
newly allocated block. Elements pushed to the back of the list are placed in
later unused slots in the original array.

Concatenating lists becomes very easy since they can just be connected to one
another using pointers.

There should be a function that consolidates all currently used elements into a
single 

# more thinky
elements should have their own structure that stores additional information to
go with any data structure

at minimum this would have to be:
- a field for the data
- a field for 

# useful operations on certain data structures
## maps/sets
- getting a list of keys
- getting a list of values
- getting a list of key-value pairs

# silly things
The above structure could be used to mimic the behavior of many data structures
with little space overhead. Since pointers will be at least 4-byte aligned on
32-bit systems, we can use the lower two bits of one of the addresses to store
information about the data structure at no extra cost.

Linked lists and array lists are the obvious ones, but you might be able to
squeeze a binary tree implementation out of it by using a different node structure

# detecting errors
Where should the error field of the data structure be set?
The validation function could set it or it could return the error and require
it to be set in the calling location. The only issue is that the caller would
have no way of knowing if the passed list is NULL. Additionally, the node
functions would also have to return their error instead of just setting it
themselves.

```
struct sub_list {
    sub_list *prev;
    sub_list *next;
    void *data;
    size_t size;
    size_t max_size;
    size_t start;
}

struct generic_list {
   sub_list *anchor;
   size_t size;
};
```

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
