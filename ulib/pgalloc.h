#ifndef ulib__pgalloc_h
#define ulib__pgalloc_h 1

#include "defs.h"

BEGIN_DECLS

/* Return the allocator page size.  */
unsigned int ulib_pgsize (void);

/* Allocate a page, aligned on a page size boundary.  */
void *ulib_pgalloc (void);

/* Release the page at PTR.  The memory should have been previously
   allocated via a call to ``ulib_pgalloc''.  */
void ulib_pgfree (void *ptr);

END_DECLS

#endif /* ulib__pgalloc_h */

/*
 * Local variables:
 * mode: C
 * indent-tabs-mode: nil
 * arch-tag: 30dc79bc-a7cd-48c5-94d7-61df2c051956
 * End:
 */
