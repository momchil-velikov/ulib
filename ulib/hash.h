#ifndef ulib__hash_h
#define ulib__hash_h 1

#include "defs.h"
#include "list.h"
#include "ulib-if.h"

BEGIN_DECLS

/* Hash function type.  */
typedef unsigned int (*ulib_hash_func) (const ulib_list *);

/* Compare function type.  */
typedef int (*ulib_hashcmp_func) (const ulib_list *, const ulib_list *);

/* Visit function type.  */
typedef int (*ulib_visit_func) (ulib_list *, void *);

/* Hash table.  */
struct ulib_hash
{
  /* Hash directory length.  */
  unsigned int length;

  /* Hash directory.  */
  ulib_list *dir;

  /* Hash function.  */
  ulib_hash_func hash;

  /* Compare function.  */
  ulib_hashcmp_func cmp;
};
typedef struct ulib_hash ulib_hash;

/* Initialize a hash table.  */
ULIB_IF int ulib_hash_init (ulib_hash *, unsigned int,
                            ulib_hash_func, ulib_hashcmp_func);

/* Destroy a hash table.  */
ULIB_IF void ulib_hash_destroy (ulib_hash *);

/* Search into the hash table.  */
ULIB_IF ulib_list *ulib_hash_lookup (const ulib_hash *, const ulib_list *);

/* Insert into the hash table.  */
ULIB_IF void ulib_hash_insert (ulib_hash *, ulib_list *);

/* Visit each element of the hash table.  Invoke the function VISIT
   with the first parameter being the element and the second being the
   user data DATA.  */
ULIB_IF void ulib_hash_traverse (ulib_hash *, ulib_visit_func visit,
                                 void *data);

/* Good string hash functions.  */
ULIB_IF unsigned int ulib_strhash (const char *);
ULIB_IF unsigned int ulib_strnhash (const char *, unsigned int);

END_DECLS

#endif /*  ulib__hash_h */

/*
 * Local variables:
 * mode: C
 * indent-tabs-mode: nil
 * arch-tag: 6a8e300b-0cd2-4db8-8070-77831d4e7e51
 * End:
 */
