#include "defs.h"

BEGIN_DECLS

#ifndef ULIB_SPLAY_TREE_KEY_TYPE
#define ULIB_SPLAY_TREE_KEY_TYPE unsigned int
#endif

#ifndef ULIB_SPLAY_TREE_COMPARE
#define ULIB_SPLAY_TREE_COMPARE(a,b) ((int) ((a) - (b)))
#endif

#ifndef ULIB_SPLAY_TREE_TYPE
#define ULIB_SPLAY_TREE_TYPE ulib_splay_tree
#endif

#ifndef ULIB_STATIC
#define ULIB_STATIC
#endif

#define ULIB___SPLAY_TREE(a,b) a##_##b
#define ULIB__SPLAY_TREE(a,b) ULIB___SPLAY_TREE(a,b)
#define ULIB_SPLAY_TREE(x) ULIB__SPLAY_TREE (ULIB_SPLAY_TREE_TYPE, x)

struct ULIB_SPLAY_TREE_TYPE
{
  struct ULIB_SPLAY_TREE_TYPE *left;
  struct ULIB_SPLAY_TREE_TYPE *right;
  ULIB_SPLAY_TREE_KEY_TYPE key;
#ifdef ULIB_SPLAY_TREE_DATA_TYPE
  ULIB_SPLAY_TREE_DATA_TYPE data;
#endif
};
typedef struct ULIB_SPLAY_TREE_TYPE ULIB_SPLAY_TREE_TYPE;

/* Splay a tree. */
ULIB_STATIC ULIB_SPLAY_TREE_TYPE *ULIB_SPLAY_TREE (splay) (
			ULIB_SPLAY_TREE_TYPE *, ULIB_SPLAY_TREE_KEY_TYPE);

/* Insert a node into the tree.  */
ULIB_STATIC int ULIB_SPLAY_TREE (insert) (ULIB_SPLAY_TREE_TYPE **,
					  ULIB_SPLAY_TREE_TYPE *);

/* Delete a key from the tree.  */
ULIB_STATIC ULIB_SPLAY_TREE_TYPE *ULIB_SPLAY_TREE (delete) (
		ULIB_SPLAY_TREE_TYPE **, ULIB_SPLAY_TREE_KEY_TYPE);

/* Lookup a key in the tree.  */
#ifdef ULIB_SPLAY_TREE_DATA_TYPE
static inline ULIB_SPLAY_TREE_DATA_TYPE *
#else
static inline int
#endif
ULIB_SPLAY_TREE (lookup) (ULIB_SPLAY_TREE_TYPE **r,
			  ULIB_SPLAY_TREE_KEY_TYPE k)
{
  *r = ULIB_SPLAY_TREE (splay) (*r, k);
  if (*r != 0 && ULIB_SPLAY_TREE_COMPARE ((*r)->key, k) == 0)
#ifdef ULIB_SPLAY_TREE_DATA_TYPE
    return &(*r)->data;
#else
    return 1;
#endif
  else
    return 0;
}

END_DECLS

/*
 * Local variables:
 * mode: C
 * indent-tabs-mode: nil
 * arch-tag: a87ff610-1e43-43fd-989d-5f13da7b0874
 * End:
 */
