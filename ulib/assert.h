#ifndef ulib__assert_h
#define ulib__assert_h 1

#include "unwind.h"

BEGIN_DECLS

#ifndef ENABLE_CHECKS
#define ENABLE_CHECKS 0
#endif

#if ENABLE_CHECKS
#define ulib___assert(file, line, expr)			\
  ulib_unwind_throw_ptr (ULIB_ASSERTION_FAILED,		\
			 "File: " file			\
			 ", line: " #line		\
			 " -- assertion failed: " #expr)

#define ulib__assert(file, line, expr) ulib___assert (file, line, expr)

#define ulib_assert(expr)						\
  ((void) ((expr) ? 1 : (ulib__assert (__FILE__, __LINE__, expr), 0)))

#else /* !ENABLE_CHECKS */

#define ulib_assert(expr) ((void) 0)

#endif /* ENABLE_CHECKS */

END_DECLS

#endif /* ulib__assert_h */

/*
 * Local variables:
 * mode: C
 * indent-tabs-mode: nil
 * arch-tag: 60240fb2-ea1b-4956-9b4c-ec1f9aeffba6
 * End:
 */
