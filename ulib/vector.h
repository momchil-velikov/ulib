#ifndef ulib__vector_h
#define ulib__vector_h 1

#include "defs.h"
#include "assert.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>

BEGIN_DECLS

/* Vector attributes.  */
#define ULIB_DATA_PTR_VECTOR 1
#define ULIB_FUNC_PTR_VECTOR 2
#define ULIB_ELT_SIZE        3
#define ULIB_RESERVE         4
#define ULIB_GROWTH_SCALE    5
#define ULIB_GROWTH_OFFSET   6

#define ULIB__ELT_SIZE_MAX     10
#define ULIB__GROWTH_SCALE_MAX  2
#define ULIB__GROWTH_OFFSET_MAX 5

struct ulib_vector
{
  /* Is it a data pointer vector.  */
  unsigned int is_data_ptr : 1;

  /* Is it a function pointer vector.  */
  unsigned int is_func_ptr : 1;

  /* Element size.  */
  unsigned int elt_size : ULIB__ELT_SIZE_MAX;

  /* Growth parameters.  When insufficient space is available, the new
     length of the vector is computed as

     max (required_length, scale * old_length + offset */
  unsigned int scale : ULIB__GROWTH_SCALE_MAX;
  unsigned int offset : ULIB__GROWTH_OFFSET_MAX;

  /* Number of elements in the vector.  */
  unsigned int nelt;

  /* Available space in the vector.  */
  unsigned int navail;

  /* Vector elements.  */
  void *data;
};
typedef struct ulib_vector ulib_vector;

/* Initialize a vector.  On error sets errno and returns a negative
   value.  */
int ulib_vector_init (ulib_vector *, ...);

/* Destroy a vector.  */
static inline void
ulib_vector_destroy (ulib_vector *v)
{
  free (v->data);
}

/* Get number of elements in the vector.  */
static inline unsigned int
ulib_vector_length (const ulib_vector *v)
{
  return v->nelt;
}

/* Set the number of elements in the vector.  */
int ulib_vector_set_size (ulib_vector *, unsigned int);

/* Ensure vector V has at least N elements.  */
int ulib_vector_atleast (ulib_vector *v, unsigned int n);

/* Change the number of elements in the vector by adding signed N to
   the current size.  */
static inline int
ulib_vector_resize (ulib_vector *v, int n)
{
  return ulib_vector_set_size (v, v->nelt + n);
}

/* Preallocate space in the vector.  */
int ulib_vector_reserve (ulib_vector *, unsigned int);

/* Empty a vector.  */
static inline void
ulib_vector_clear (ulib_vector *v, int release)
{
  v->nelt = 0;
  if (release)
    {
      free (v->data);
      v->data = 0;
    }
}

/* Set a vector element.  */
int ulib_vector_set (ulib_vector *, unsigned int, const void *);

/* Get a pointer to a vector element.  */
static inline void *
ulib_vector_elt (const ulib_vector *v, unsigned int idx)
{
  assert (idx < v->nelt);
  return (char *) v->data + idx * v->elt_size;
}

/* Get a pointer to the first element.  */
static inline void *
ulib_vector_front (const ulib_vector *v)
{
  return v->data;
}

/* Get a pointer one past the last element. */
static inline void *
ulib_vector_back (const ulib_vector *v)
{
  return (char *) v->data + v->nelt * v->elt_size;
}

/* Set a data pointer element.  */
int ulib_vector_set_ptr (ulib_vector *, unsigned int, void *);

/* Get the value of a data pointer element.  */
static inline void *
ulib_vector_ptr_elt (const ulib_vector *v, unsigned int idx)
{
  assert (idx < v->nelt);
  return ((void **) v->data) [idx];
}

/* Set a function pointer element.  */
int ulib_vector_set_func (ulib_vector *, unsigned int, ulib_func);

/* Get the value of a function pointer element.  */
static inline ulib_func
ulib_vector_func_elt (const ulib_vector *v, unsigned int idx)
{
  assert (idx < v->nelt);
  return ((ulib_func *) v->data) [idx];
}

/* Append an element to the vector.  */
static inline int
ulib_vector_append (ulib_vector *v, const void *data)
{
  return ulib_vector_set (v, v->nelt, data);
}

/* Append a data pointer to the vector.  */
static inline int
ulib_vector_append_ptr (ulib_vector *v, void *data)
{
  return ulib_vector_set_ptr (v, v->nelt, data);
}

/* Append a function pointer to the vector.  */
static inline int
ulib_vector_append_func (ulib_vector *v, ulib_func func)
{
  return ulib_vector_set_func (v, v->nelt, func);
}

/* private */
int ulib__vector_moveup (ulib_vector *v, unsigned int idx);

/* Insert an element into the vector before position IDX.  */
static inline int
ulib_vector_insert (ulib_vector *v, unsigned int idx, const void *data)
{
  if (ulib__vector_moveup (v, idx) < 0)
    return -1;
  memcpy ((char *) v->data + idx * v->elt_size, data, v->elt_size);
  v->nelt++;
  return 0;
}

/* Insert a data pointer into the vector before position IDX.  */
static inline int
ulib_vector_insert_ptr (ulib_vector *v, unsigned int idx, void *data)
{
  if (ulib__vector_moveup (v, idx) < 0)
    return -1;
  ((void **) v->data) [idx] = data;
  v->nelt++;
  return 0;
}

/* Insert a function pointer into the vector before position IDX.  */
static inline int
ulib_vector_insert_func (ulib_vector *v, unsigned int idx, ulib_func func)
{
  if (ulib__vector_moveup (v, idx) < 0)
    return -1;
  ((ulib_func *) v->data) [idx] = func;
  v->nelt++;
  return 0;
}

/* Remove the element at IDX from the vector.  */
void ulib_vector_remove (ulib_vector *v, unsigned int idx);

/* Remove the last element in the vector.  */
static inline void
ulib_vector_remove_last (ulib_vector *v)
{
  assert (v->nelt > 0);
  v->nelt--;
}

/* Copy a vector.  Both vectors must be initialized with compatible
   parameters. */
int ulib_vector_copy (ulib_vector *dst, const ulib_vector *src);

END_DECLS
#endif /* ulib__vector_h */

/*
 * Local variables:
 * mode: C
 * indent-tabs-mode: nil
 * arch-tag: d88ce214-3aa2-454c-a70b-af923b2dd2eb
 * End:
 */
