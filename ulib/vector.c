#include "vector.h"
#include "unwind.h"
#include <stdarg.h>
#include <string.h>

/* Initialize a vector.  */
void
ulib_vector_init (ulib_vector *v, ...)
{
  va_list ap;
  unsigned int attr, value;

  va_start (ap, v);

  memset (v, 0, sizeof (ulib_vector));
  v->scale = 1;

  do
    {
      attr = va_arg (ap, unsigned int);
      switch (attr)
	{
	case ULIB_DATA_PTR_VECTOR:
	  v->is_data_ptr = 1;
	  break;

	case ULIB_FUNC_PTR_VECTOR:
	  v->is_func_ptr = 1;
	  break;

	case ULIB_ELT_SIZE:
	  value = va_arg (ap, unsigned int);
	  if (value >= (1U << ULIB__ELT_SIZE_MAX))
	    goto einval;
	  v->elt_size = value;
	  break;

	case ULIB_RESERVE:
	  v->navail = va_arg (ap, unsigned int);
	  break;

	case ULIB_GROWTH_SCALE:
	  value = va_arg (ap, unsigned int);
	  if (value >= (1U << ULIB__GROWTH_SCALE_MAX))
	    goto einval;
	  v->scale = value;
	  break;

	case ULIB_GROWTH_OFFSET:
	  value = va_arg (ap, unsigned int);
	  if (value >= (1U << ULIB__GROWTH_OFFSET_MAX))
	    goto einval;
	  v->offset = value;
	  break;

	case 0:
	  break;

	default:
	  goto einval;
	}
    }
  while (attr);

  if (v->is_data_ptr && v->is_func_ptr)
    goto einval;
  else if (v->is_data_ptr)
    v->elt_size = sizeof (void *);
  else if (v->is_func_ptr)
    v->elt_size = sizeof (ulib_func);
  else if (v->elt_size == 0)
    goto einval;

  if (v->navail)
    v->data = ulib_calloc (v->navail, v->elt_size);
  return;

 einval:
  ulib_unwind_throw_int (ULIB_INVALID_PARAMETER, 0);
}

/* Increase the available space.  */
static void
grow (ulib_vector *v, unsigned int req)
{
  unsigned int n;

  n = v->scale * v->nelt + v->offset;
  if (req > n)
    n = req;

  v->data = ulib_realloc (v->data, n * v->elt_size);
  memset ((char *) v->data + v->navail * v->elt_size, 0,
	  (n - v->navail) * v->elt_size);
  v->navail = n;
}

/* Preallocate a number of elements in the vector.  */
void
ulib_vector_reserve (ulib_vector *v, unsigned int req)
{
  if (req > v->navail)
    grow (v, req);
}

/* Append an element to the vector.  */
void
ulib_vector_set (ulib_vector *v, unsigned int idx, const void *data)
{
  if (idx >= v->navail)
    grow (v, idx + 1);

  memcpy ((char *) v->data + idx * v->elt_size, data, v->elt_size);
  if (idx >= v->nelt)
    v->nelt = idx + 1;
}

/* Set a data pointer element.  */
void
ulib_vector_set_ptr (ulib_vector *v, unsigned int idx, void *data)
{
  if (idx >= v->navail)
    grow (v, idx + 1);

  ((void **) v->data) [idx] = data;
  if (idx >= v->nelt)
    v->nelt = idx + 1;
}

/* Set a function pointer element.  */
void
ulib_vector_set_func (ulib_vector *v, unsigned int idx, ulib_func func)
{
  if (idx >= v->navail)
    grow (v, idx + 1);

  ((ulib_func *) v->data) [idx] = func;
  if (idx >= v->nelt)
    v->nelt = idx + 1;
}

/* Helper function to make available slot number IDX by moving
   elements [IDX, NELT) up.  */
void
ulib__vector_moveup (ulib_vector *v, unsigned int idx)
{
  if (idx >= v->navail)
    grow (v, idx + 1);

  if (idx < v->nelt)
    {
      char *dst = (char *) v->data + idx * v->elt_size;
      memmove (dst + v->elt_size, dst, v->nelt - idx);
    }
}

/* Remove the element at IDX from the vector.  */
void
ulib_vector_remove (ulib_vector *v, unsigned int idx)
{
  char *dst;

  ulib_assert (idx < v->nelt);
  v->nelt--;
  if (idx < v->nelt)
    {
      dst = (char *) v->data + idx * v->elt_size;
      memmove (dst, dst + v->elt_size, v->nelt - idx);
    }
}

/*
 * Local variables:
 * mode: C
 * indent-tabs-mode: nil
 * arch-tag: 32517a6f-d411-4504-9013-cc0225752c04
 * End:
 */
