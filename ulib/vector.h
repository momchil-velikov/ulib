#ifndef ulib__vector_h
#define ulib__vector_h 1

#include "defs.h"
#include "alloc.h"
#include "assert.h"
#include <string.h>

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

/* Initialize a vector.  Throws ULIB_INVALID_PARAMETER.  */
void ulib_vector_init (ulib_vector *, ...);

/* Destroy a vector.  */
static inline void
ulib_vector_destroy (ulib_vector *v)
{
  ulib_free (v->data);
}

/* Get number of elements in the vector.  */
static inline unsigned int
ulib_vector_length (const ulib_vector *v)
{
  return v->nelt;
}

/* Preallocate a number of elements in the vector.  Can throw
   ULIB_NO_MEMORY.  */
void ulib_vector_reserve (ulib_vector *, unsigned int);

/* Empty a vector.  */
static inline void
ulib_vector_clear (ulib_vector *v, int release)
{
  v->nelt = 0;
  if (release)
    {
      ulib_free (v->data);
      v->data = 0;
    }
}

/* Set a vector element.  */
void ulib_vector_set (ulib_vector *, unsigned int, const void *);

/* Get a pointer to a vector element.  */
static inline const void *
ulib_vector_elt (const ulib_vector *v, unsigned int idx)
{
  ulib_assert (idx < v->nelt);
  return (char *) v->data + idx * v->elt_size;
}

/* Get a pointer to the last element. */
static inline void *
ulib_vector_last_elt (const ulib_vector *v)
{
  ulib_assert (v->nelt > 0);
  return (char *) v->data + (v->nelt - 1) * v->elt_size;
}

/* Set a data pointer element.  */
void ulib_vector_set_ptr (ulib_vector *, unsigned int, void *);

/* Get the value of a data pointer element.  */
static inline void *
ulib_vector_ptr_elt (const ulib_vector *v, unsigned int idx)
{
  ulib_assert (idx < v->nelt);
  return ((void **) v->data) [idx];
}

/* Get the value of the last data pointer element.  */
static inline void *
ulib_vector_last_ptr_elt (const ulib_vector *v)
{
  ulib_assert (v->nelt > 0);
  return ((void **) v->data) [v->nelt - 1];
}

/* Set a function pointer element.  */
void ulib_vector_set_func (ulib_vector *, unsigned int, ulib_func);

/* Get the value of a function pointer element.  */
static inline ulib_func
ulib_vector_func_elt (const ulib_vector *v, unsigned int idx)
{
  ulib_assert (idx < v->nelt);
  return ((ulib_func *) v->data) [idx];
}

/* Get the value of the last function poiner element.  */
static inline ulib_func
ulib_vector_last_func_elt (const ulib_vector *v)
{
  ulib_assert (v->nelt > 0);
  return ((ulib_func *) v->data) [v->nelt - 1];
}

/* Append an element to the vector.  */
static inline void
ulib_vector_append (ulib_vector *v, const void *data)
{
  ulib_vector_set (v, v->nelt, data);
}

/* Append a data pointer to the vector.  */
static inline void
ulib_vector_append_ptr (ulib_vector *v, void *data)
{
  ulib_vector_set_ptr (v, v->nelt, data);
}

/* Append a function pointer to the vector.  */
static inline void
ulib_vector_append_func (ulib_vector *v, ulib_func func)
{
  ulib_vector_set_func (v, v->nelt, func);
}

/* private */
void ulib__vector_moveup (ulib_vector *v, unsigned int idx);

/* Insert an element into the vector before position IDX.  */
static inline void
ulib_vector_insert (ulib_vector *v, unsigned int idx, const void *data)
{
  ulib__vector_moveup (v, idx);
  memcpy ((char *) v->data + idx * v->elt_size, data, v->elt_size);
  v->nelt++;
}

/* Insert a data pointer into the vector before position IDX.  */
static inline void
ulib_vector_insert_ptr (ulib_vector *v, unsigned int idx, void *data)
{
  ulib__vector_moveup (v, idx);
  ((void **) v->data) [idx] = data;
  v->nelt++;
}

/* Insert a function pointer into the vector before position IDX.  */
static inline void
ulib_vector_insert_func (ulib_vector *v, unsigned int idx, ulib_func func)
{
  ulib__vector_moveup (v, idx);
  ((ulib_func *) v->data) [idx] = func;
  v->nelt++;
}

/* Remove the element at IDX from the vector.  */
void ulib_vector_remove (ulib_vector *v, unsigned int idx);

/* Remove the last element in the vector.  */
static inline void
ulib_vector_remove_last (ulib_vector *v)
{
  ulib_assert (v->nelt > 0);
  v->nelt--;
}

END_DECLS

#endif /* ulib__vector_h */

/*
 * Local variables:
 * mode: C
 * indent-tabs-mode: nil
 * arch-tag: d88ce214-3aa2-454c-a70b-af923b2dd2eb
 * End:
 */
