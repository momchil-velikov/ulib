#include <ulib/cache.h>

#define ULIB_SPLAY_TREE_KEY_TYPE unsigned int
#define ULIB_SPLAY_TREE_TYPE uint_tree

#include <ulib/splay-tree.h>
#include <ulib/splay-tree.c>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

static ulib_cache *uint_tree_cache;

static void
uint_tree_ctor (void *_obj, unsigned int size __attribute__ ((unused)))
{
  uint_tree *obj = (uint_tree *) _obj;

  obj->left = obj->right = 0;
}

static int
uint_tree_scan (void *_obj, void **objs, unsigned int n)
{
  uint_tree *obj = (uint_tree *) _obj;
  unsigned int cnt;
  
  cnt = !!obj->left + !!obj->right;
  if (cnt > n)
    return -cnt;

  cnt = 0;
  if (obj->left)
    objs [cnt++] = obj->left;
  if (obj->right)
    objs [cnt++] = obj->right;
  return cnt;
}

#define NLOOPS 10000000
#define NKEYS 1000
#define GCSTEP 1000

static void check_less_than (uint_tree *node, unsigned int key);
static void check_greater_than (uint_tree *node, unsigned int key);

static void
check_less_than (uint_tree *node, unsigned int key)
{
  if (node->key >= key)
    abort ();

  if (node->left)
    check_less_than (node->left, key);

  if (node->right)
    check_less_than (node->right, key);
}

static void
check_greater_than (uint_tree *node, unsigned int key)
{
  if (node->key <= key)
    abort ();

  if (node->left)
    check_greater_than (node->left, key);

  if (node->right)
    check_greater_than (node->right, key);
}

static void
check_tree (uint_tree *node)
{
  if (node->left)
    {
      check_less_than (node->left, node->key);
      check_tree (node->left);
    }

  if (node->right)
    {
      check_greater_than (node->right, node->key);
      check_tree (node->right);
    }
}

static unsigned int key [NKEYS + 1];
static uint_tree *root;
static unsigned int nins, ndel, ngc;

static int
scan_root (void *_obj, void **objs, unsigned int n)
{
  void **obj = (void **) _obj;

  if (*obj == 0)
    return 0;

  if (n == 0)
    return -1;

  objs [0] = *obj;
  return 1;
}

static void
test ()
{
  unsigned int i, idx;
  uint_tree *elt;

  ulib_gcroot (&root, scan_root);

  for (i = 0; i < NLOOPS; i++)
    {
      idx = rand () % NKEYS + 1;
      if (key [idx])
	{
	  uint_tree_delete (&root, key [idx]);
	  ndel++;
	  key [idx] = 0;
	}

      elt = ulib_cache_alloc (uint_tree_cache);
      elt->key = key [idx] = rand () % NKEYS + 1;
      while (uint_tree_insert (&root, elt))
	{
	  nins++;
	  elt = uint_tree_delete (&root, key [idx]);
	  ndel++;
	  elt->key = key [idx] = rand () % NKEYS + 1;
	}
      nins++;

      if ((i + 1) % GCSTEP == 0)
	{
	  ulib_gcrun ();
	  ngc++;
	}
    }

  for (i = 1; i <= NKEYS; i++)
    if (key [i])
      {
	uint_tree_delete (&root, key [i]);
	ndel++;
      }

  ulib_gcrun ();
}

int
main ()
{
  struct timeval tv1, tv2;
  double tm;

  setvbuf (stdout, 0, _IONBF, 0);

  uint_tree_cache =
    ulib_cache_create (ULIB_CACHE_SIZE, sizeof (uint_tree),
		       ULIB_CACHE_ALIGN, sizeof (void *),
		       ULIB_CACHE_CTOR, uint_tree_ctor,
		       ULIB_CACHE_GCSCAN, uint_tree_scan,
		       0);
  gettimeofday (&tv1, 0);
  test ();
  gettimeofday (&tv2, 0);

  if (root)
    check_tree (root);

  tm = tv2.tv_sec + tv2.tv_usec / 1e6 - tv1.tv_sec - tv1.tv_usec / 1e6;

  printf ("time = %f s\n", tm);
  printf ("nins = %d, ndel = %d\n", nins, ndel);
  printf ("nalloc = %d\n", NLOOPS);
  printf ("avg alloc+ins+del+gc = %f us\n",
	  1e6 * tm / (NLOOPS + nins + ndel + ngc));
  return 0;
}

/*
 * Local variables:
 * mode: C
 * indent-tabs-mode: nil
 * arch-tag: 112d0e33-1ee4-4cf9-837b-f344fb5ae93d
 * End:
 */
