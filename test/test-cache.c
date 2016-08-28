#include <ulib/cache.h>
#include <ulib/rand.h>
#include <ulib/time.h>

#include <stdio.h>
#include <stdlib.h>

#define NLOOP 1000000
#define NPTR 5000
#define NCACHE 20

static void *ptr[NCACHE][NPTR];
static ulib_cache *cache[NCACHE];

int
main ()
{
  ulib_time ts1, ts2;
  unsigned int size, cidx, pidx, i;
  double tm;
  unsigned int count = 0, alloc = 0;
  
  ulib_gettime (&ts1);
  for (size = 8, cidx = 0; cidx < NCACHE; cidx++)
    {
      size *= 1.2;
      cache [cidx] = ulib_cache_create (ULIB_CACHE_SIZE, size,
					ULIB_CACHE_ALIGN, 4,
					0);

      for (pidx = 0; pidx < NLOOP; pidx++)
	{
	  i = ulib_rand (0, NPTR - 1);
	  if (ptr [cidx][i] != 0)
	    {
	      ulib_cache_free (cache [cidx], ptr [cidx][i]);
	      ptr [cidx][i] = 0;
	    }

	  ptr [cidx][i] = ulib_cache_alloc (cache [cidx]);
	  count++;
	  if (ptr [cidx] [i])
	    alloc += size;
	}

      for (i = 0; i < NPTR; i++)
	if (ptr [cidx][i])
	  ulib_cache_free (cache [cidx], ptr [cidx][i]);

      ulib_cache_flush (cache [cidx]);
    }
  ulib_gettime (&ts2);

  tm = ts2.sec * 1e6 + ts2.usec - ts1.sec * 1e6 - ts1.usec;

  alloc /= 1048576;
  printf ("calls = %u, alloc = %u MiB, time = %f s\n",
	  count, alloc, tm / 1e6);
  printf ("avg %f us per alloc/free\n", tm / count);
  return 0;
}

/*
 * Local variables:
 * mode: C
 * indent-tabs-mode: nil
 * End:
 */
