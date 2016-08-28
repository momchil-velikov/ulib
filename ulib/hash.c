#include "hash.h"
#include <stdlib.h>
#include <limits.h>

/* Initialize a hash table.  */
int
ulib_hash_init (ulib_hash *table, unsigned int length,
		ulib_hash_func hash, ulib_hashcmp_func cmp)
{
  unsigned int i;

  if ((table->dir = calloc (length, sizeof (ulib_list))) == 0)
    return -1;

  table->length = length;
  table->hash = hash;
  table->cmp = cmp;

  i = length;
  while (i--)
    ulib_list_init (table->dir + i);

  return 0;
}

/* Destroy a hash table.  */
void
ulib_hash_destroy (ulib_hash *table)
{
  free (table->dir);
}

/* Search into the hash table.  */
ulib_list *
ulib_hash_lookup (const ulib_hash *table, const ulib_list *key)
{
  ulib_list *lst;
  unsigned int idx = table->hash (key) % table->length;

  for (lst = table->dir [idx].next;
       lst != (void *) (table->dir + idx);
       lst = lst->next)
    if (table->cmp (key, lst) == 0)
      return lst;

  return 0;
}

/* Insert into the hash table.  */
void
ulib_hash_insert (ulib_hash *table, ulib_list *key)
{
  unsigned int idx = table->hash (key) % table->length;

  ulib_list_append (table->dir + idx, key);
}

/* Visit each element of the hash table.  Invoke the function VISIT
   with the first parameter being the element and the second being the
   user data DATA.  */
void
ulib_hash_traverse (ulib_hash *table, ulib_visit_func visit, void *data)
{
  unsigned int i;
  ulib_list *elt;

  i = table->length;
  while (i--)
    for (elt = table->dir [i].next;
	 elt != (void *) &table->dir [i];
	 elt = elt->next)
      if (visit (elt, data))
	break;
}

/* Good strings hash function.  */
#define SHIFT (sizeof (int) * CHAR_BIT - 4)

unsigned int
ulib_strhash (const char *str)
{
  unsigned int h = 0, g;

  while (*str)
    {
      h = (h << 4) + *str++;
      if ((g = h & (0xf << SHIFT)) != 0)
	{
	  h = h ^ (g >> (SHIFT - 4));
	  h = h ^ g;
	}
    }

  return h;
}

unsigned int
ulib_strnhash (const char *str, unsigned int n)
{
  unsigned int i, h = 0, g;

  for (i = 0; i < n; i++)
    {
      h = (h << 4) + str [i];
      if ((g = h & (0xf << SHIFT)) != 0)
	{
	  h = h ^ (g >> (SHIFT - 4));
	  h = h ^ g;
	}
    }

  return h;
}

/*
 * Local variables:
 * mode: C
 * indent-tabs-mode: nil
 * End:
 */
