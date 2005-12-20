#ifndef ulib__log_h
#define ulib__log_h 1

#include "defs.h"
#include "vector.h"
#include "ulib-if.h"
#include <stdio.h>
#include <stdarg.h>

BEGIN_DECLS

/* Message log structure.  */
struct ulib_log
{
  /* Logging domain.  */
  char *domain;

  /* Messages array.  */
  ulib_vector msgs;
};
typedef struct ulib_log ulib_log;

/* Initialize a log.  */
ULIB_IF int ulib_log_init (ulib_log *log, const char *domain);

/* Destroy a log.  */
ULIB_IF void ulib_log_destroy (ulib_log *);

/* Get number of messages in the log.  */
ULIB_IF unsigned int ulib_log_length (const ulib_log *);

/* Get Nth message from the log.  */
ULIB_IF const char *ulib_log_getmsg (const ulib_log *, unsigned int);

/* Append a message to the log.  */
ULIB_IF int ulib_log_printf (ulib_log *log, const char *fmt, ...);
ULIB_IF int ulib_log_vprintf (ulib_log *log, const char *fmt, va_list);

/* Remove the oldest N messages from the log.  Remove all messages
   if N is zero.  */
ULIB_IF void ulib_log_clear (ulib_log *log, unsigned int n);

/* Output messages in the log to the IO stream STREAM.  */
ULIB_IF void ulib_log_write (ulib_log *log, FILE *stream);

/* Output messages in the log to the file descriptor FD.  */
ULIB_IF void ulib_log_writefd (ulib_log *log, int fd);

END_DECLS

#endif /* ulib__log_h */

/*
 * Local variables:
 * mode: C
 * indent-tabs-mode: nil
 * arch-tag: ff781dc2-086d-4f99-a512-a7f40a063c26
 * End:
 */
