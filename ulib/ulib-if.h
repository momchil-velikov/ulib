#ifndef ulib__ulib_if_h
#define ulib__ulib_if_h 1

#include "defs.h"

#ifdef ULIB_SO
# define ULIB_IF ULIB_EXPORT
#else
# define ULIB_IF ULIB_IMPORT
#endif

#endif /* ulib__ulib_if_h */

/*
 * Local variables:
 * mode: C
 * indent-tabs-mode: nil
 * arch-tag: 74cb5ec2-67b2-4635-9310-66e2ea244871
 * End:
 */
