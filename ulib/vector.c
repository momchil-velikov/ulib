#include "vector.h"
#include <stdarg.h>
#include <string.h>
#include <errno.h>

/* Initialize a vector.  */
int
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
    {
      v->data = calloc (v->navail, v->elt_size);
      if (v->data == 0)
        {
          errno = ENOMEM;
          return -1;
        }
    }
  return 0;

 einval:
  errno = EINVAL;
  return -1;
}

/* Increase the available space.  */
static int
grow (ulib_vector *v, unsigned int req)
{
  unsigned int n;
  void *data;

  n = v->scale * v->nelt + v->offset;
  if (req > n)
    n = req;

  if ((data = realloc (v->data, n * v->elt_size)) != 0)
    {
      v->data = data;
      memset ((char *) v->data + v->navail * v->elt_size, 0,
              (n - v->navail) * v->elt_size);
      v->navail = n;

      return 0;
    }

  return -1;
}

/* Set the number of elements in the vector.  */
int
ulib_vector_set_size (ulib_vector *v, unsigned int sz)
{
  if (sz > v->navail)
    {
      if (grow (v, sz) < 0)
        return -1;
    }
  else if (sz > v->nelt)
    {
      memset ((char *) v->data + v->nelt * v->elt_size, 0,
              (sz - v->nelt) * v->elt_size);
      
    }
  
  v->nelt = sz;
  return 0;
}

/* Ensure vector V has at least N elements.  */
int
ulib_vector_atleast (ulib_vector *v, unsigned int n)
{
  if (n <= v->nelt)
    return 0;
  else
    return ulib_vector_set_size (v, n);
}

/* Preallocate a number of elements in the vector.  */
int
ulib_vector_reserve (ulib_vector *v, unsigned int req)
{
  if (req > v->navail)
    return grow (v, req);
  else
    return 0;
}

/* Append an element to the vector.  */
int
ulib_vector_set (ulib_vector *v, unsigned int idx, const void *data)
{
  if (idx >= v->navail && grow (v, idx + 1) < 0)
    return -1;

  memcpy ((char *) v->data + idx * v->elt_size, data, v->elt_size);
  if (idx >= v->nelt)
    v->nelt = idx + 1;
  return 0;
}

/* Set a data pointer element.  */
int
ulib_vector_set_ptr (ulib_vector *v, unsigned int idx, void *data)
{
  if (idx >= v->navail && grow (v, idx + 1) < 0)
    return -1;

  ((void **) v->data) [idx] = data;
  if (idx >= v->nelt)
    v->nelt = idx + 1;
  return 0;
}

/* Set a function pointer element.  */
int
ulib_vector_set_func (ulib_vector *v, unsigned int idx, ulib_func func)
{
  if (idx >= v->navail && grow (v, idx + 1) < 0)
    return -1;

  ((ulib_func *) v->data) [idx] = func;
  if (idx >= v->nelt)
    v->nelt = idx + 1;
  return 0;
}

/* Helper function to make available slot number IDX by moving
   elements [IDX, NELT) up.  */
int
ulib__vector_moveup (ulib_vector *v, unsigned int idx)
{
  if (idx >= v->navail && grow (v, idx + 1) < 0)
    return -1;

  if (idx < v->nelt)
    {
      char *dst = (char *) v->data + idx * v->elt_size;
      memmove (dst + v->elt_size, dst, v->nelt - idx);
    }

  return 0;
}

/* Remove the element at IDX from the vector.  */
void
ulib_vector_remove (ulib_vector *v, unsigned int idx)
{
  char *dst;

  assert (idx < v->nelt);
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
