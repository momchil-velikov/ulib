#include "pgalloc.h"
#include "list.h"

#include <stdlib.h>
#include <inttypes.h>

#define NPAGES 31
#define ALLOC_MASK 0x7fffffff
#define BONUS_ALLOC_MASK 0x80000000

struct pgroup
{
  /* List of all the page groups.  */
  ulib_list list;

  /* First page aligned address.  */
  void *page;

  /* Allocation bitmap: set - available, clear - allocated.  */
  unsigned int map;
};

#define ULIB_SPLAY_TREE_KEY_TYPE void *

#define ULIB_SPLAY_TREE_COMPARE(a,b)		\
  ((const char *) (a) - (const char *) (b))

#define ULIB_SPLAY_TREE_DATA_TYPE struct pgroup

#define ULIB_SPLAY_TREE_TYPE pgroup_tree

#include "splay-tree.h"

#include "splay-tree.c"

/* Allocator data.  */
static struct
{
  /* Linked list of all the page groups.  */
  ulib_list groups;

  /* Root of the page group tree.  */
  struct pgroup_tree *root;

  /* First page group with unallocated pages.  */
  struct pgroup *free;

  /* Allocator page size. */
  unsigned int pgsize;
} G = { {0, 0}, 0, 0, 4096 };

/* Initialize the page allocator.  */
static void
pgalloc_init ()
{
  G.free = (struct pgroup *) &G.groups;
  ulib_list_init (G.free);
}

/* Return the allocator page size.  */
unsigned int
ulib_pgsize ()
{
  return G.pgsize;
}

/* Allocate a page, aligned on a page size boundary from group
   GRP.  */
static const unsigned char mapidx [] =
{
  /* 0000 */ 0,
  /* 0001 */ 0,
  /* 0010 */ 1,
  /* 0011 */ 0,
  /* 0100 */ 2,
  /* 0101 */ 0,
  /* 0110 */ 1,
  /* 0111 */ 0,
  /* 1000 */ 3,
  /* 1001 */ 0,
  /* 1010 */ 1,
  /* 1011 */ 0,
  /* 1100 */ 2,
  /* 1101 */ 0,
  /* 1110 */ 1,
  /* 1111 */ 0
};

static void *
pgalloc (struct pgroup *grp)
{
  unsigned int i, mask;

  i = 0;

  mask = 0xffff;
  if ((grp->map & mask) == 0)
    {
      i += 16;
      mask <<= 16;
    }

  mask = mask & (mask >> 8);
  if ((grp->map & mask) == 0)
    {
      i += 8;
      mask <<= 8;
    }

  mask = mask & (mask >> 4);
  if ((grp->map & mask) == 0)
    {
      i += 4;
      mask <<= 4;
    }

  i = i + mapidx [(grp->map & mask) >> i];
  return (char *) grp->page + i * G.pgsize;
}

/* Allocate a page, aligned on a page size boundary.  */
void *
ulib_pgalloc ()
{
  void *ptr;
  struct pgroup *grp;
  struct pgroup_tree *node;

  /* Ensure allocator is initialized.  FIXME: thread safety.  */
  static int initialized = 0;
  if (!initialized)
    {
      pgalloc_init ();
      initialized = 1;
    }

  /* Check if there's a non-empty group.  */
  grp = G.free;
  if (grp != (struct pgroup *) &G.groups)
    {
      ptr = pgalloc (grp);

      /* If the group is emptied, move it before the free groups
	 pointer. */
      if (grp->map == 0)
	{
	  ulib_list_remove (&grp->list);
	  ulib_list_insert (G.free, &grp->list);
	}
      return ptr;
    }

  /* Allocate a new page group.  */
  if ((ptr = malloc ((NPAGES + 1) * G.pgsize)) != 0)
    {
      if ((node = malloc (sizeof (struct pgroup_tree))) != 0)
        {
          node->left = node->right = 0;
          node->key = ptr;

          /* Check if the malloc returned a suitably aligned page.  It
             is quite likely such a big allocations resulted in a
             pointer aligned on the system's page size boundary and
             our page size is usually smaller.  If this is the case,
             we have one bonus page, besides the usual NPAGES.  */
          if (((uintptr_t) ptr & (G.pgsize - 1)) == 0)
            {
              node->data.page = ptr;
              node->data.map = BONUS_ALLOC_MASK | ALLOC_MASK;
            }
          else
            {
              node->data.page =
                (void *) (((uintptr_t) ptr + G.pgsize - 1) & -G.pgsize);
              node->data.map = ALLOC_MASK;
            }
          
          ulib_list_insert (&G.groups, &node->data.list);
          pgroup_tree_insert (&G.root, node);
          
          return pgalloc (&node->data);
        }
      free (ptr);
    }
  return 0;
}

/* Mark as available from GRP the page at PTR.  */
static inline void
pgfree (struct pgroup *grp, void *ptr)
{
  grp->map |= 1U << (((char *) ptr - (char *) grp->page) / G.pgsize);
}

/* Release the page at PTR.  The memory should have been previously
   allocated via a call to ``ulib_pgalloc''.  */
void
ulib_pgfree (void *ptr)
{
  struct pgroup_tree *grp;
  unsigned int omap;

  /* Find the group, which contains the block. */
  grp = G.root = pgroup_tree_splay (G.root, ptr);
  while (1)
    {
      if (ULIB_SPLAY_TREE_COMPARE (ptr, grp->key) < 0)
	grp = grp->left;
      else if (ULIB_SPLAY_TREE_COMPARE (ptr, (char *) grp->key + G.pgsize)
	       >= 0)
	grp = grp->right;
      else
	break;
    }

  /* Release pages.  */
  omap = grp->data.map;
  pgfree (&grp->data, ptr);

  /* Deallocate the page group if it became full.  */
  if ((grp->data.map & ALLOC_MASK) == ALLOC_MASK
      && (grp->key != grp->data.page
	  || grp->data.map == (BONUS_ALLOC_MASK | ALLOC_MASK)))
    {
      grp = pgroup_tree_delete (&G.root, grp->key);
      if (G.free == &grp->data)
	G.free = (struct pgroup *) grp->data.list.next;
      ulib_list_remove (&grp->data.list);
      free (grp->key);
      free (grp);
    }
  /* Insert the page group at the head of the free list, if it was
     empty.  */
  else if (omap == 0)
    {
      ulib_list_remove (&grp->data.list);
      ulib_list_insert (G.free, &grp->data.list);
      G.free = (struct pgroup *) &grp->data;
    }
}

/*
 * Local variables:
 * mode: C
 * indent-tabs-mode: nil
 * End:
 */
