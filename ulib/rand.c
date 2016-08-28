#include "rand.h"

/* Generate a random permutation of the elements of the vector V of
   length N.  */
void
ulib_shuffle (unsigned int n, unsigned int *v)
{
  unsigned int i, r, tmp;

  for (i = 0; i < n; ++i)
    {
      r = ulib_rand (0, n - 1);
      tmp = v [r];
      v [r] = v [i];
      v [i] = tmp;
    }
}

/*
 * Local variables:
 * mode: C
 * indent-tabs-mode: nil
 * End:
 */

