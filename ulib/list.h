#ifndef ulib__list_h
#define ulib__list_h 1

#include "defs.h"

BEGIN_DECLS

struct ulib_list
{
  struct ulib_list *next;
  struct ulib_list *prev;
};
typedef struct ulib_list ulib_list;

static inline void
ulib_list_init (void *ptr)
{
  ulib_list *lp = (ulib_list *) ptr;
  lp->next = lp->prev = lp;
}

static inline void
ulib_list_append (void *lst, void *ptr)
{
  ulib_list *lp = (ulib_list *) lst;
  ulib_list *bp = (ulib_list *) ptr;

  bp->next = lp->next;
  bp->prev = lp;
  lp->next->prev = bp;
  lp->next = bp;
}

static inline void
ulib_list_insert (void *lst, void *ptr)
{
  ulib_list *lp = (ulib_list *) lst;
  ulib_list *bp = (ulib_list *) ptr;

  bp->next = lp;
  bp->prev = lp->prev;
  lp->prev->next = bp;
  lp->prev = bp;
}

static inline void
ulib_list_remove (void *ptr)
{
  ulib_list *bp = (ulib_list *) ptr;

  bp->next->prev = bp->prev;
  bp->prev->next = bp->next;
  bp->next = bp->prev = bp;
}

static inline int
ulib_list_empty_p (const void *ptr)
{
  const ulib_list *bp = (const ulib_list *) ptr;
  return bp->next == bp && bp->prev == bp;
}

END_DECLS

#endif /* ulib__list_h */

/*
 * Local variables:
 * mode: C
 * indent-tabs-mode: nil
 * arch-tag: 3354e636-0332-49e7-8996-2b5fcea5788c
 * End:
 */
