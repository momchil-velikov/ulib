#ifndef ulib__alloc_h
#define ulib__alloc_h 1

#include "defs.h"

BEGIN_DECLS

/* Allocate space for an object whose size is specified by SIZE.
   Throws ULIB_NO_MEMORY.  */
void *ulib_malloc (size_t size);

/* Allocates space for an array of N objects, each of whose size is
   SIZE.  The space is initialized to all bits zero.  Throws
   ULIB_NO_MEMORY.  */
void *ulib_calloc (size_t n, size_t size);

/* Allocate space for an object whose size is specified by SIZE.  The
   space is initialized to all bits zero.  Throws ULIB_NO_MEMORY.  */
static inline void *
ulib_zalloc (size_t size)
{
  return ulib_calloc (1, size);
}

/* Deallocate the old object pointed to by PTR and return a pointer to
   a new object that has the size specified by SIZE.  Throws
   ULIB_NO_MEMORY. */
void *ulib_realloc (void *ptr, size_t size);

/* Cause the space pointed to by PTR to be deallocated.  */
void ulib_free (void *ptr);

END_DECLS

#endif /* ulib__alloc_h */

/*
 * Local variables:
 * mode: C
 * indent-tabs-mode: nil
 * arch-tag: e59aa0d3-5226-47e7-9543-2ba1fba74b42
 * End:
 */
