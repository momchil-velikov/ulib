#include <ulib/pgalloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#define NLOOP 1000000
#define NPTR 1000

static void *ptr[NPTR];

static unsigned int
calibrate ()
{
  unsigned int i, idx, count = 0;

  for (i = 0; i < NLOOP; i++)
    {
      idx = rand () % NPTR;
      if (ptr [idx] != 0)
	ptr [idx] = 0;

      ptr [idx] = (void *) 1;
      if (ptr [idx])
	count++;
    }

  for (i = 0; i < NPTR; i++)
    if (ptr [i])
      ptr [i] = 0;

  return count;
}

static unsigned int
test ()
{
  unsigned int i, idx, count = 0;

  for (i = 0; i < NLOOP; i++)
    {
      idx = rand () % NPTR;
      if (ptr [idx] != 0)
	{
	  ulib_pgfree (ptr [idx]);
	  ptr [idx] = 0;
	}

      ptr [idx] = ulib_pgalloc ();
      if (ptr [idx])
	{
	  count++;
#if 0
	  memset (ptr [idx], idx % 256, ulib_pgsize ());
#endif
	}
    }

  for (i = 0; i < NPTR; i++)
    if (ptr [i])
      {
	ulib_pgfree (ptr [i]);
	ptr [i] = 0;
      }

  return count;
}

int
main ()
{
  unsigned int count, alloc;
  struct timeval tv1, tv2;
  double over, tm;

  gettimeofday (&tv1, 0);
  count = calibrate ();
  gettimeofday (&tv2, 0);
  over = tv2.tv_sec * 1e6 + tv2.tv_usec - tv1.tv_sec * 1e6 - tv1.tv_usec;

  gettimeofday (&tv1, 0);
  count = test ();
  gettimeofday (&tv2, 0);
  tm = tv2.tv_sec * 1e6 + tv2.tv_usec - tv1.tv_sec * 1e6 - tv1.tv_usec;
  tm -= over;
  alloc = count * ulib_pgsize () / 1048576;

  printf ("count = %u, alloc = %u MiB, time = %f, real = %f s\n",
	  count, alloc, tm / 1e6, (tm + over) / 1e6);
  printf ("avg alloc/free = %f us per call\n", tm / count);
  printf ("avg alloc/free = %f us per MiB\n", tm / alloc);
  return 0;
}

/*
 * Local variables:
 * mode: C
 * indent-tabs-mode: nil
 * arch-tag: a2cec68e-6458-4caa-ac81-c22c1451feca
 * End:
 */
