#ifndef ulib__defs_h
#define ulib__defs_h 1

#ifdef _MSC_VER
# define inline __inline
#endif

#if defined (__GNUC__)
# define restrict __restrict__
# define inline __inline__
#elif defined (__ICC)
# define restrict restrict
#else
# define restrict
#endif

#ifndef __GNUC__
#define __attribute__(x)
#endif

#ifndef BEGIN_DECLS
# ifdef __cplusplus
#  define BEGIN_DECLS extern "C" {
#  define END_DECLS }
# else
#  define BEGIN_DECLS
#  define END_DECLS
# endif
#endif

typedef void (*ulib_func)();

#include <stddef.h>

#endif /* ulib__defs_h */

/*
 * Local variables:
 * mode: C
 * indent-tabs-mode: nil
 * arch-tag: 3a0d4cb3-eed7-4db5-9928-195aa7ddb6eb
 * End:
 */
