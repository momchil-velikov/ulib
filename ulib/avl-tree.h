#include "defs.h"

BEGIN_DECLS

#ifndef ULIB_AVL_TREE_KEY_TYPE
#define ULIB_AVL_TREE_KEY_TYPE unsigned int
#endif

#ifndef ULIB_AVL_TREE_COMPARE
#define ULIB_AVL_TREE_COMPARE(a,b) ((int) ((a) - (b)))
#endif

#ifndef ULIB_AVL_TREE_TYPE
#define ULIB_AVL_TREE_TYPE ulib_avl_tree
#endif

#ifndef ULIB_STATIC
#define ULIB_STATIC
#endif

#define ULIB___AVL_TREE(a,b) a##_##b
#define ULIB__AVL_TREE(a,b) ULIB___AVL_TREE(a,b)
#define ULIB_AVL_TREE(x) ULIB__AVL_TREE (ULIB_AVL_TREE_TYPE, x)

struct ULIB_AVL_TREE_TYPE
{
  struct ULIB_AVL_TREE_TYPE *left;
  struct ULIB_AVL_TREE_TYPE *right;
  ULIB_AVL_TREE_KEY_TYPE key;
  int balance;
#ifdef ULIB_AVL_TREE_DATA_TYPE
  ULIB_AVL_TREE_DATA_TYPE data;
#endif
};
typedef struct ULIB_AVL_TREE_TYPE ULIB_AVL_TREE_TYPE;

/* Insert a node into the tree pointed to by PR. Returns negative if
   the key is already present.  */
ULIB_STATIC int ULIB_AVL_TREE (insert) (ULIB_AVL_TREE_TYPE **pr,
					ULIB_AVL_TREE_TYPE *key);

static int
ULIB_AVL_TREE (del) (ULIB_AVL_TREE_TYPE **pr,
		     ULIB_AVL_TREE_KEY_TYPE key, ULIB_AVL_TREE_TYPE **dn);

/* Remove a key from an AVL tree. Return the removed node or null if
   the key is not found.  */
ULIB_STATIC inline ULIB_AVL_TREE_TYPE *
ULIB_AVL_TREE (delete) (ULIB_AVL_TREE_TYPE **pr, ULIB_AVL_TREE_KEY_TYPE key)
{
  ULIB_AVL_TREE_TYPE *dn;

  return ULIB_AVL_TREE (del) (pr, key, &dn) < 0 ? 0 : dn;
}

/* Lookup a key in the tree.  */
#ifdef ULIB_AVL_TREE_DATA_TYPE
ULIB_STATIC ULIB_AVL_TREE_DATA_TYPE *
#else
ULIB_STATIC int
#endif
ULIB_AVL_TREE (lookup) (const ULIB_AVL_TREE_TYPE *r,
			ULIB_AVL_TREE_KEY_TYPE key);

/*
 * Local variables:
 * mode: C
 * indent-tabs-mode: nil
 * arch-tag: 04eca3c3-03ec-4b83-be20-77d663bbb9b6
 * End:
 */
