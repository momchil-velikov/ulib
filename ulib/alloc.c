#include "alloc.h"
#include "unwind.h"
#include <stdlib.h>

/* Allocate space for an object whose size is specified by SIZE.
   Throws ULIB_NO_MEMORY.  */
void *
ulib_malloc (size_t size)
{
  void *ptr;

  if ((ptr = malloc (size)))
    return ptr;
  ulib_unwind_throw_int (ULIB_NO_MEMORY, 0);
  return 0;
}

/* Allocates space for an array of N objects, each of whose size is
   SIZE.  The space is initialized to all bits zero.  Throws
   ULIB_NO_MEMORY.  */
void *
ulib_calloc (size_t n, size_t size)
{
  void *ptr;

  if ((ptr = calloc (n, size)))
    return ptr;
  ulib_unwind_throw_int (ULIB_NO_MEMORY, 0);
  return 0;
}

/* Deallocate the old object pointed to by PTR and return a pointer to
   a new object that has the size specified by SIZE.  Throws
   ULIB_NO_MEMORY. */
void *
ulib_realloc (void *ptr, size_t size)
{
  void *new;

  if ((new = realloc (ptr, size)))
    return new;
  ulib_unwind_throw_int (ULIB_NO_MEMORY, 0);
  return 0;
}

/* Cause the space pointed to by PTR to be deallocated.  */
void
ulib_free (void *ptr)
{
  free (ptr);
}

/*
 * Local variables:
 * mode: C
 * indent-tabs-mode: nil
 * arch-tag: 11efefa5-61fb-4ef0-8e6a-26f4e4be22ac
 * End:
 */
