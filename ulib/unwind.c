#include "unwind.h"
#include "alloc.h"
#include <assert.h>
#include <stdlib.h>

/* FIXME: Not thread safe - either pthread get/set specific or TLS (or
   don't care *grin*).  */
struct ulib__unwind_rec_base *ulib__current_unwind_record;

/* Next available unuque ID.  */
static unsigned int id = ULIB__LAST_EXCEPTION;

/* Request COUNT exception type IDs.  Return the start of the
   range.  */
unsigned int
unwind_request_id (unsigned int count)
{
  unsigned int n;

  n = id;
  id += count;
  return n;
}

void
ulib_unwind_cleanup_add (ulib_unwind_func f, void *a)
{
  struct ulib__unwind_rec_cleanup *r;

  r = ulib_malloc (sizeof (struct ulib__unwind_rec_cleanup));
  r->base.flags = ULIB__UNWIND_MALLOC;
  r->func = f;
  r->arg = a;

  r->base.prev = ulib__current_unwind_record;
  ulib__current_unwind_record = (struct ulib__unwind_rec_base *) r;
}

void
ulib__unwind_cleanup_add (struct ulib__unwind_rec_cleanup *r,
			  ulib_unwind_func f, void *a)
{
  r->base.flags = 0;
  r->func = f;
  r->arg = a;

  r->base.prev = ulib__current_unwind_record;
  ulib__current_unwind_record = (struct ulib__unwind_rec_base *) r;
}

void
ulib_unwind_cleanup_del (int run)
{
  struct ulib__unwind_rec_base *rb;

  while ((rb = ulib__current_unwind_record)
	 && (rb->flags & (ULIB__UNWIND_FRAME | ULIB__UNWIND_CATCH)) == 0)
    {
      ulib__current_unwind_record = rb->prev;

      if (run)
	{
	  struct ulib__unwind_rec_cleanup *rc;

	  rc = (struct ulib__unwind_rec_cleanup *) rb;
      	  rc->func (rc->arg);
	}

      if (rb->flags & ULIB__UNWIND_MALLOC)
	free (rb);
    }
}

void
ulib__unwind_throw (int type, const ulib_throw_arg *arg)
{
  struct ulib__unwind_rec_base *rb;
  struct ulib__unwind_rec_catch *rc;

  while ((rb = ulib__current_unwind_record)
	 && (rb->flags & ULIB__UNWIND_CATCH) == 0)
    {
      ulib__current_unwind_record = rb->prev;

      if ((rb->flags & ULIB__UNWIND_FRAME) == 0)
	{
	  struct ulib__unwind_rec_cleanup *rc;

	  rc = (struct ulib__unwind_rec_cleanup *) rb;
	  rc->func (rc->arg);

	  if (rb->flags & ULIB__UNWIND_MALLOC)
	    free (rb);
	}
    }

  if (!(rb && (rb->flags & ULIB__UNWIND_CATCH)))
    ulib_unwind_unhandled_exception (type, arg);

  assert (rb && (rb->flags & ULIB__UNWIND_CATCH));
  rc = (struct ulib__unwind_rec_catch *) rb;
  rc->type = type;
  rc->arg = *arg;
  longjmp (rc->jmpbuf, 1);
}

/* Default unhandled exception function.  */
static void ulib__default_unhandled_exception (
					int, const ulib_throw_arg *data)
     __attribute__ ((noreturn));

static void
ulib__default_unhandled_exception (
		int type __attribute__ ((unused)),
		const ulib_throw_arg *data __attribute__ ((unused)))
{
  abort ();
}

/* Pointer to a function to be invoked for unhandled exceptions.  */
void (*ulib_unwind_unhandled_exception) (int type,
					 const ulib_throw_arg *data)
     __attribute__ ((noreturn)) = ulib__default_unhandled_exception;

/*
 * Local variables:
 * mode: C
 * indent-tabs-mode: nil
 * arch-tag: febd55b8-0146-4a62-9809-c44e4391755f
 * End:
 */
