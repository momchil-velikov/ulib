#ifndef ulib__unwind_h
#define ulib__unwind_h 1

#include <setjmp.h>
#include "defs.h"

BEGIN_DECLS

typedef void (*ulib_unwind_func) (void *);

/* Exception argument type - portable throw of integer, function and
   data pointer values.  */
union ulib_throw_arg
{
  int integer;
  void *data_ptr;
  void (*func_ptr)();
};
typedef union ulib_throw_arg ulib_throw_arg;

/* Standard exception types.  */
#define ULIB_NO_MEMORY         0
#define ULIB_SYSTEM_ERROR      1
#define ULIB_ASSERTION_FAILED  2
#define ULIB_INVALID_PARAMETER 3
#define ULIB__LAST_EXCEPTION   4

/* Request COUNT exception type IDs.  Return the start of the
   range.  */
unsigned int unwind_request_id (unsigned int count);

/* Register a cleanup handler function FUNC to be invoked with
   argument ARG.  */
void ulib_unwind_cleanup_add (ulib_unwind_func func, void *arg);

/* Pop the unwind cleanups up to an unwind frame.  Execute cleanups
   iff RUNP is true.  */
void ulib_unwind_cleanup_del (int runp);

void ulib__unwind_throw (int type, const ulib_throw_arg *data)
     __attribute__ ((noreturn));

/* Throw an exception of type TYPE.  Pass an arbitrary data item DATA
   to the exception handler.  */
static void ulib_unwind_throw_int (int type, int data)
     __attribute__ ((noreturn));
static inline void
ulib_unwind_throw_int (int type, int data)
{
  ulib_throw_arg arg;
  arg.integer = data;
  ulib__unwind_throw (type, &arg);
}

static void ulib_unwind_throw_ptr (int type, void *data)
     __attribute__ ((noreturn));
static inline void
ulib_unwind_throw_ptr (int type, void *data)
{
  ulib_throw_arg arg;
  arg.data_ptr = data;
  ulib__unwind_throw (type, &arg);
}

static void ulib_unwind_throw_func (int type, void (*data) ())
     __attribute__ ((noreturn));
static inline void
ulib_unwind_throw_func (int type, void (*data) ())
{
  ulib_throw_arg arg;
  arg.func_ptr = data;
  ulib__unwind_throw (type, &arg);
}

/* Pointer to a function to be invoked for unhandled exceptions.  */
extern void (*ulib_unwind_unhandled_exception) (
				int type, const ulib_throw_arg *data)
     __attribute__ ((noreturn));

#define ULIB__UNWIND_FRAME  1
#define ULIB__UNWIND_CATCH  2
#define ULIB__UNWIND_MALLOC 4

struct ulib__unwind_rec_base
{
  int flags;
  struct ulib__unwind_rec_base *prev;
};

struct ulib__unwind_rec_cleanup
{
  struct ulib__unwind_rec_base base;
  ulib_unwind_func func;
  void *arg;
};

struct ulib__unwind_rec_catch
{
  struct ulib__unwind_rec_base base;
  jmp_buf jmpbuf;
  unsigned int type;
  ulib_throw_arg arg;
};

extern struct ulib__unwind_rec_base *ulib__current_unwind_record;

void ulib__unwind_cleanup_add (struct ulib__unwind_rec_cleanup *,
			       ulib_unwind_func, void *);

/* Run cleanup handlers.  */
#define ulib_unwind_cleanup_run() \
  ulib_unwind_cleanup_del (1)

/* Discard cleanup handlers.  */
#define ulib_unwind_cleanup_discard() \
  ulib_unwind_cleanup_del (0)

/* Begin an unwind frame.  Cleanup handlers execution stops at unwind
   frame boundary.  */
#define ulib_unwind_frame_begin						\
  do									\
    {									\
      struct ulib__unwind_rec_base ulib__unwind_rec_base_buffer;	\
      ulib__unwind_rec_base_buffer.flags = ULIB__UNWIND_FRAME;		\
      ulib__unwind_rec_base_buffer.prev = ulib__current_unwind_record;	\
      ulib__current_unwind_record = &ulib__unwind_rec_base_buffer

/* Finish an unwind frame.  All the cleanup handlers pushed within the
   unwind frame are executed.  */
#define ulib_unwind_frame_end						\
    ulib_unwind_cleanup_run ();						\
    ulib__current_unwind_record = ulib__unwind_rec_base_buffer.prev;	\
  } while (0)

/* Begin a try block. All the exceptions throw in locations
   dynamically enclosed by this try block are handled in the
   corresponding catch clause.  The try block is a beginning of an
   unwind frame.  */
#define ulib_unwind_try							\
  do									\
  {									\
    struct ulib__unwind_rec_catch ulib__unwind_rec_catch_buffer;	\
    ulib__unwind_rec_catch_buffer.base.flags				\
       = ULIB__UNWIND_FRAME | ULIB__UNWIND_CATCH;			\
    ulib__unwind_rec_catch_buffer.base.prev				\
       = ulib__current_unwind_record;					\
    ulib__current_unwind_record = &ulib__unwind_rec_catch_buffer.base;	\
    if (setjmp (ulib__unwind_rec_catch_buffer.jmpbuf) == 0)

/* Begin a catch block.  */
#define ulib_unwind_catch(type, arg)				\
    else							\
    {								\
      unsigned int type = ulib__unwind_rec_catch_buffer.type;	\
      ulib_throw_arg arg = ulib__unwind_rec_catch_buffer.arg;

/* End a try/catch block.  The end block finishes the unwind frame
   instroduced by the try block.  */
#define ulib_unwind_end_catch				\
    }							\
    ulib_unwind_cleanup_run ();				\
    ulib__current_unwind_record				\
       = ulib__unwind_rec_catch_buffer.base.prev;	\
  } while (0)

END_DECLS

#endif /* ulib__unwind_h */

/*
 * Local variables:
 * mode: C
 * indent-tabs-mode: nil
 * arch-tag: 96a65167-abc0-400c-a00b-0e939842b873
 * End:
 */
