#include "log.h"
#include <stdarg.h>
#include <errno.h>
#include <unistd.h>

/* Initialize a log.  */
int
ulib_log_init (ulib_log *log, const char *domain)
{
  if (ulib_vector_init (&log->msgs, ULIB_DATA_PTR_VECTOR, 0) == 0)
    {
      log->domain = malloc (strlen (domain) + 1);
      if (log->domain != 0)
        {
          strcpy (log->domain, domain);
          return 0;
        }
      ulib_vector_destroy (&log->msgs);
    }
  errno = ENOMEM;
  return -1;
}

/* Destroy a log.  */
void
ulib_log_destroy (ulib_log *log)
{
  unsigned int i, n;

  if (log->domain)
    free (log->domain);

  n = ulib_vector_length (&log->msgs);
  for (i = 0; i < n; ++i)
    {
      char *msg = ulib_vector_ptr_elt (&log->msgs, i);
      if (msg)
        free (msg);
    }
  ulib_vector_destroy (&log->msgs);
}

/* Get number of messages in the log.  */
unsigned int
ulib_log_length (const ulib_log *log)
{
  return ulib_vector_length (&log->msgs);
}

/* Get Nth message from the log.  */
const char *
ulib_log_message_get (const ulib_log *log, unsigned int idx)
{
  return ulib_vector_ptr_elt (&log->msgs, idx);
}

/* Append a message to the log.  */
int
ulib_log_message_add (ulib_log *log, const char *fmt, ...)
{
  const int DEFAULT_BUFSZ = 80;
  va_list ap;
  char *buf;
  int n;

  if ((buf = malloc (DEFAULT_BUFSZ)) == 0)
    {
      errno = ENOMEM;
      return -1;
    }

  va_start (ap, fmt);
  n = vsnprintf (buf, DEFAULT_BUFSZ, fmt, ap);
  va_end (ap);

  if (n < 0)
    goto error;
  
  if (n >= DEFAULT_BUFSZ)
    {
      char *nbuf;
      if ((nbuf = realloc (buf, n + 1)) == 0)
        goto enomem;

      va_start (ap, fmt);
      n = vsnprintf (nbuf, n, fmt, ap);
      va_end (ap);

      if (n < 0)
        goto error;

      if (ulib_vector_append_ptr (&log->msgs, buf) < 0)
        goto error;
    }

  return 0;

enomem:
  errno = ENOMEM;
error:
  free (buf);
  return -1;
}

/* Remove the oldest N messages from the log.  Remove all messages
   when N is zero.  */
void
ulib_log_clear (ulib_log *log, unsigned int n)
{
  char *msg;
  unsigned int i, j;

  if (n == 0)
    n = ulib_vector_length (&log->msgs);

  for (i = 0; i < n; ++i)
    {
      msg = ulib_vector_ptr_elt (&log->msgs, i);
      ulib_vector_set_ptr (&log->msgs, i, 0);
      free (msg);
    }

  n = ulib_vector_length (&log->msgs);
  for (j = 0; i < n; ++i, ++j)
    {
      msg = ulib_vector_ptr_elt (&log->msgs, i);
      ulib_vector_set_ptr (&log->msgs, i, 0);
      ulib_vector_set_ptr (&log->msgs, j, msg);
    }
  
  ulib_vector_set_size (&log->msgs, j);
}

/* Output messages in the log to the IO stream STREAM.  */
void
ulib_log_print (ulib_log *log, FILE *stream)
{
  ulib_log_fdprint (log, fileno (stream));
}

/* Output messages in the log to the file descriptor FD.  */
void
ulib_log_fdprint (ulib_log *log, int fd)
{
  const char *msg;
  unsigned int i, n;

  n = ulib_vector_length (&log->msgs);
  for (i = 0; i < n; ++i)
    {
      msg = ulib_vector_ptr_elt (&log->msgs, i);
      if (log->domain)
        write (fd, log->domain, strlen (log->domain));
      else
        write (fd, "<domain>", sizeof ("<domain>") - 1);
      write (fd, ": ", sizeof (": ") - 1);
      write (fd, msg, strlen (msg));
      write (fd, "\n", sizeof ("\n") - 1);
    }
}


/*
 * Local variables:
 * mode: C
 * indent-tabs-mode: nil
 * arch-tag: ef0fa690-ef5f-4934-82aa-d8d0d8bc8c23
 * End:
 */
