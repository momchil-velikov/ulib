#include <ulib/bitset.h>
#include <ulib/rand.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/time.h>

#define BITS_MAX 1024U
#define BITS_PER_SET 500
#define NSETS_MAX 100

#ifdef NDEBUG
#define NLOOPS 10000000
#else
#define NLOOPS 100000
#endif

static ulib_bitset *set [NSETS_MAX];

/* Generate a random set of NBITS elements.  */
static void
random_set (unsigned int nbits, ulib_bitset *set)
{
  unsigned int i, n;

  for (i = 0; i < nbits; ++i)
    {
      n = ulib_rand (0, BITS_MAX - 1);
      ulib_bitset_set (set, n);
    }
}

/* Allocate and populate a random set.  */
static ulib_bitset *
make_random_set (unsigned int nbits)
{
  ulib_bitset *set;

  set = malloc (sizeof (ulib_bitset));
  ulib_bitset_init (set);
  random_set (nbits, set);
  
  return set;
}

/* Check whether DST is the union of SRC1 and SRC2.  */
static int
check_union (const ulib_bitset *dst, const ulib_bitset *src1,
             const ulib_bitset *src2)
{
  unsigned int i, n, m;

  n = ulib_bitset_max (src1);
  m = ulib_bitset_max (src2);

  n = n > m ? n : m;

  /* Check whether each bit in SRC1 or SRC2 in present in DST.  */
  for (i = 0; i < n; ++i)
    if ((ulib_bitset_isset (src1, i) != 0
         || ulib_bitset_isset (src2, i) != 0)
        && ulib_bitset_isset (dst, i) == 0)
      return -1;

  /* Check whether each bit in DST is present in SRC1 or SRC2.  */
  n = ulib_bitset_max (dst);
  for (i = 0; i < n; ++i)
    if (ulib_bitset_isset (dst, i) != 0
        && ulib_bitset_isset (src1, i) == 0
        && ulib_bitset_isset (src2, i) == 0)
      return -1;

  return 0;
}

/* Check whether DST is the intersection of SRC1 and SRC2.  */
static int
check_intersection (const ulib_bitset *dst, const ulib_bitset *src1,
                    const ulib_bitset *src2)
{
  unsigned int i, n, m;

  n = ulib_bitset_max (src1);
  m = ulib_bitset_max (src2);

  n = n > m ? n : m;

  /* Check whether a bit, set in both SRC1 and SRC2, is set in DST
     too.  */
  for (i = 0; i < n; ++i)
    if (ulib_bitset_isset (src1, i) != 0
        && ulib_bitset_isset (src2, i) != 0
        && ulib_bitset_isset (dst, i) == 0)
      return -1;

  /* Check whether a bit, set in DST, is set in SRC1 and SRC2 too.  */
  n = ulib_bitset_max (dst);
  for (i = 0; i < n; ++i)
    if (ulib_bitset_isset (dst, i) != 0
        && (ulib_bitset_isset (src1, i) == 0
            || ulib_bitset_isset (src2, i) == 0))
      return -1;

  return 0;
}

/* Check whether DST is the set difference between SRC1 and SRC2.  */
static int
check_difference (const ulib_bitset *dst, const ulib_bitset *src1,
                  const ulib_bitset *src2)
{
  unsigned int i, n, m;

  n = ulib_bitset_max (src1);
  m = ulib_bitset_max (src2);

  n = n > m ? n : m;

  /* Check whether each bit in SRC2 is not present in DST.  */
  for (i = 0; i < n; ++i)
    if (ulib_bitset_isset (src2, i) != 0
        && ulib_bitset_isset (dst, i) != 0)
      return -1;

  /* Check whether each bit in DST is set in SRC1 and clear in
     SRC2.  */
  n = ulib_bitset_max (dst);
  for (i = 0; i < n; ++i)
    if (ulib_bitset_isset (dst, i) != 0
        && (ulib_bitset_isset (src1, i) == 0
            || ulib_bitset_isset (src2, i) != 0))
      return -1;

  return 0;
}

/* Allocate and populate the test sets.  */
static void
init_sets ()
{
  unsigned int i;

  for (i = 0; i < NSETS_MAX; ++i)
    set [i] = make_random_set (BITS_PER_SET);
}

/* Destroy the test sets.  */
static void
destroy_sets ()
{
  unsigned int i;

  for (i = 0; i < NSETS_MAX; ++i)
    {
      ulib_bitset_destroy (set [i]);
      free (set [i]);
    }
}


/* Test union operation.  */
static void
test_bitset_union ()
{
  volatile int sts;
  unsigned int i, d, s1, s2;

  for (i = 0; i < NLOOPS; ++i)
    {
      do
        {
          d = ulib_rand (0, NSETS_MAX - 1);
          s1 = ulib_rand (0, NSETS_MAX - 1);
          s2 = ulib_rand (0, NSETS_MAX - 1);
        }
      while (d == s1 || d == s2 || s1 == s2);

      sts = ulib_bitset_union (set [d], set [s1], set [s2]);
      assert (sts == 0);

      assert (check_union (set [d], set [s1], set [s2]) == 0);
    }
}

static void
test_bitset_union_inplace ()
{
  volatile int sts;
  unsigned int i, d, s;

  for (i = 0; i < NLOOPS; ++i)
    {
      do
        {
          d = ulib_rand (0, NSETS_MAX - 1);
          s = ulib_rand (0, NSETS_MAX - 1);
        }
      while (d == s);

      sts = ulib_bitset_union_inplace (set [d], set [s]);
      assert (sts == 0);

      assert (check_union (set [d], set [d], set [s]) == 0);
    }
}


/* Test intersection operation.  */
static void
test_bitset_intersection()
{
  volatile int sts;
  unsigned int i, d, s1, s2;

  for (i = 0; i < NLOOPS; ++i)
    {
      do
        {
          d = ulib_rand (0, NSETS_MAX - 1);
          s1 = ulib_rand (0, NSETS_MAX - 1);
          s2 = ulib_rand (0, NSETS_MAX - 1);
        }
      while (d == s1 || d == s2 || s1 == s2);

      sts = ulib_bitset_intersection (set [d], set [s1], set [s2]);
      assert (sts == 0);

      assert (check_intersection (set [d], set [s1], set [s2]) == 0);
    }
}

static void
test_bitset_intersection_inplace ()
{
  volatile int sts;
  unsigned int i, d, s;

  for (i = 0; i < NLOOPS; ++i)
    {
      do
        {
          d = ulib_rand (0, NSETS_MAX - 1);
          s = ulib_rand (0, NSETS_MAX - 1);
        }
      while (d == s);

      sts = ulib_bitset_intersection_inplace (set [d], set [s]);
      assert (sts == 0);

      assert (check_intersection (set [d], set [d], set [s]) == 0);
    }
}


/* Test difference operation.  */
static void
test_bitset_difference ()
{
  volatile int sts;
  unsigned int i, d, s1, s2;

  for (i = 0; i < NLOOPS; ++i)
    {
      do
        {
          d = ulib_rand (0, NSETS_MAX - 1);
          s1 = ulib_rand (0, NSETS_MAX - 1);
          s2 = ulib_rand (0, NSETS_MAX - 1);
        }
      while (d == s1 || d == s2 || s1 == s2);

      sts = ulib_bitset_difference (set [d], set [s1], set [s2]);
      assert (sts == 0);

      assert (check_difference (set [d], set [s1], set [s2]) == 0);
    }
}

static void
test_bitset_difference_inplace ()
{
  volatile int sts;
  unsigned int i, d, s;

  for (i = 0; i < NLOOPS; ++i)
    {
      do
        {
          d = ulib_rand (0, NSETS_MAX - 1);
          s = ulib_rand (0, NSETS_MAX - 1);
        }
      while (d == s);

      sts = ulib_bitset_difference_inplace (set [d], set [s]);
      assert (sts == 0);

      assert (check_difference (set [d], set [d], set [s]) == 0);
    }
}


/* Compute timestamp difference. Result in microsseconds.  */
static double
diffts (const struct timeval *begin, const struct timeval *end)
{
  return (end->tv_sec * 1e6 + end->tv_usec
          - begin->tv_sec * 1e6 - begin->tv_usec);
}

int
main ()
{
  struct timeval ts1, ts2;

  init_sets ();
  gettimeofday (&ts1, 0);
  test_bitset_union ();
  gettimeofday (&ts2, 0);
  destroy_sets ();
  printf ("union: %f s\n", diffts (&ts1, &ts2) / 1e6);

  init_sets ();
  gettimeofday (&ts1, 0);
  test_bitset_union_inplace ();
  gettimeofday (&ts2, 0);
  destroy_sets ();
  printf ("union[in-place]: %f s\n", diffts (&ts1, &ts2) / 1e6);

  init_sets ();
  gettimeofday (&ts1, 0);
  test_bitset_intersection ();
  gettimeofday (&ts2, 0);
  destroy_sets ();
  printf ("intersection: %f s\n", diffts (&ts1, &ts2) / 1e6);

  init_sets ();
  gettimeofday (&ts1, 0);
  test_bitset_intersection_inplace ();
  gettimeofday (&ts2, 0);
  destroy_sets ();
  printf ("intersection[in-place]: %f s\n", diffts (&ts1, &ts2) / 1e6);

  init_sets ();
  gettimeofday (&ts1, 0);
  test_bitset_difference ();
  gettimeofday (&ts2, 0);
  destroy_sets ();
  printf ("difference: %f s\n", diffts (&ts1, &ts2) / 1e6);

  init_sets ();
  gettimeofday (&ts1, 0);
  test_bitset_difference_inplace ();
  gettimeofday (&ts2, 0);
  destroy_sets ();
  printf ("difference[in-place]: %f s\n", diffts (&ts1, &ts2) / 1e6);
  
  return 0;
}

/*
 * Local variables:
 * mode: C
 * indent-tabs-mode: nil
 * arch-tag: 228f20b7-011f-43b7-a851-84d4f7e20816
 * End:
 */
