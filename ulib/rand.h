#ifndef ulib__rand_h
#define ulib__rand_h 1

#include "defs.h"
#include <stdlib.h>
#include <limits.h>

BEGIN_DECLS

/* Generate a pseudo random number in the range [LO, HI].  */
static inline unsigned int
ulib_rand (unsigned int lo, unsigned int hi)
{
  return (unsigned int) (lo + (hi - lo) * (rand () / (double) RAND_MAX));
}

END_DECLS
#endif /* ulib__rand_h */

/*
 * Local variables:
 * mode: C
 * indent-tabs-mode: nil
 * arch-tag: 0cc42e5e-bcfa-49f3-bd81-08ef819f4870
 * End:
 */
