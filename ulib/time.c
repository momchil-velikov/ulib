#include "time.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

/* Get current time.  */
void
ulib_gettime (ulib_time *timep)
{
#ifdef _WIN32
  FILETIME ft;
  ULARGE_INTEGER uli;

  GetSystemTimeAsFileTime (&ft);
  
  uli.LowPart = ft.dwLowDateTime;
  uli.HighPart = ft.dwHighDateTime;

  uli.QuadPart /= 10;
  
  timep->sec = uli.QuadPart / 1000000;
  timep->usec = uli.QuadPart % 1000000;
#else /* ! _WIN32 */
  struct timeval tv;

  gettimeofday (&tv, 0);

  timep->sec = tv.tv_sec;
  timep->usec = tv.tv_usec;
#endif /* _WIN32 */
}

/*
 * Local variables:
 * mode: C
 * indent-tabs-mode: nil
 * End:
 */
