#ifndef ulib__defs_h
#define ulib__defs_h 1

#ifdef _MSC_VER
#define inline __inline
#endif

#if defined(__GNUC__)
#define restrict __restrict__
#define inline __inline__
#elif defined(__ICC)
#define restrict restrict
#else
#define restrict
#endif

#ifndef __GNUC__
#define __attribute__(x)
#endif

#ifdef __GNUC__
#define ULIB_EXPORT __attribute__((visibility("default")))
#define ULIB_IMPORT
#define ULIB_INTERNAL __attribute__((visibility("internal")))
#elif defined(_MSC_VER)
#define ULIB_EXPORT __declspec(dllexport)
#define ULIB_IMPORT __declspec(dllimport)
#define ULIB_INTERNAL
#else
#define ULIB_EXPORT
#define ULIB_IMPORT
#define ULIB_INTERNAL
#endif

#ifndef BEGIN_DECLS
#ifdef __cplusplus
#define BEGIN_DECLS extern "C" {
#define END_DECLS }
#else
#define BEGIN_DECLS
#define END_DECLS
#endif
#endif

typedef void (*ulib_func)();

#include <stddef.h>

#endif /* ulib__defs_h */

/*
 * Local variables:
 * mode: C
 * indent-tabs-mode: nil
 * End:
 */
