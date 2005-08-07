#include <ulib/cache.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>

#define NLOOP 1000000
#define NPTR 5000
#define NCACHE 20

static void *ptr[NCACHE][NPTR];
static ulib_cache *cache[NCACHE];

int
main ()
{
  struct timeval tv1, tv2;
  unsigned int size, cidx, pidx, i;
  double tm;
  unsigned int count = 0, alloc = 0;

  gettimeofday (&tv1, 0);
  for (size = 8, cidx = 0; cidx < NCACHE; cidx++)
    {
      size *= 1.2;
      cache [cidx] = ulib_cache_create (ULIB_CACHE_SIZE, size,
					ULIB_CACHE_ALIGN, 4,
					0);

      for (pidx = 0; pidx < NLOOP; pidx++)
	{
	  i = rand () % NPTR;
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
  gettimeofday (&tv2, 0);

  tm = tv2.tv_sec + tv2.tv_usec / 1e6 - tv1.tv_sec - tv1.tv_usec / 1e6;

  alloc /= 1048576;
  printf ("calls = %d, alloc = %d MiB, time = %f s\n",
	  count, alloc, tm);
  printf ("avg %f us per alloc/free\n", 1e6 * tm / count);
  return 0;
}

/*
 * Local variables:
 * mode: C
 * indent-tabs-mode: nil
 * arch-tag: d9975273-4af6-4029-9422-1d9f8e24ea6b
 * End:
 */
