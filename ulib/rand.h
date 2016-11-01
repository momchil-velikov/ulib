#ifndef ulib__rand_h
#define ulib__rand_h 1

#include "defs.h"
#include "ulib-if.h"
#include <stdlib.h>
#include <limits.h>

BEGIN_DECLS

/* Generate a pseudo random number in the range [LO, HI].  */
static inline unsigned int
ulib_rand(unsigned int lo, unsigned int hi) {
    return (unsigned int)(rand() / (RAND_MAX + 1.0) * (hi - lo + 1) + lo);
}

/* Generate a random permutation of the elements of the vector V of
   length N.  */
ULIB_IF void ulib_shuffle(unsigned int n, unsigned int *v);

END_DECLS
#endif /* ulib__rand_h */

/*
 * Local variables:
 * mode: C
 * indent-tabs-mode: nil
 * End:
 */
