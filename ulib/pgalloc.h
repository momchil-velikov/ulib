#ifndef ulib__pgalloc_h
#define ulib__pgalloc_h 1

#include "defs.h"
#include "ulib-if.h"

BEGIN_DECLS

/* Return the allocator page size.  */
ULIB_IF unsigned int ulib_pgsize (void);

/* Allocate a page, aligned on a page size boundary.  */
ULIB_IF void *ulib_pgalloc (void);

/* Release the page at PTR.  The memory should have been previously
   allocated via a call to ``ulib_pgalloc''.  */
ULIB_IF void ulib_pgfree (void *ptr);

END_DECLS

#endif /* ulib__pgalloc_h */

/*
 * Local variables:
 * mode: C
 * indent-tabs-mode: nil
 * End:
 */
