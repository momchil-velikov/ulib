#ifndef ulib__time_h
#define ulib__time_h 1

#include "defs.h"
#include "ulib-if.h"

BEGIN_DECLS

/* Time structure.  */
struct ulib_time
{
  /* Seconds.  */
  unsigned int sec;

  /* Microseconds.  */
  unsigned int usec;
};
typedef struct ulib_time ulib_time;

/* Get current time.  */
ULIB_IF void ulib_gettime (ulib_time *);

END_DECLS
#endif /* ulib__time_h */

/*
 * Local variables:
 * mode: C
 * indent-tabs-mode: nil
 * arch-tag: 8f7c35a8-f3ee-4472-871b-65047af7d5c2
 * End:
 */
