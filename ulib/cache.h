#ifndef ulib__cache_h
#define ulib__cache_h

#include "defs.h"
#include "list.h"

BEGIN_DECLS

/* Minimum object size.  */
#define ULIB_CACHE_OBJECT_SIZE_MIN 8

/* Maximum object size.  We've come to this number empirically - it
   provides for reasonable memory utilization, while fitting in a
   single page.  */
#define ULIB_CACHE_OBJECT_SIZE_MAX  926

/* Minimum object alignment.  */
#define ULIB_CACHE_OBJECT_ALIGN_MIN 4

/* Maximum object alignment.  More or less arbitrary, but still large
   enough to allow for L1 cache line boundary alignments.  */
#define ULIB_CACHE_OBJECT_ALIGN_MAX 128

/* Constructor/destructor/clear function type.  */
typedef int (*ulib_ctor_func) (void *obj, unsigned int size);
typedef void (*ulib_clear_func) (void *obj, unsigned int size);
typedef void (*ulib_dtor_func) (void *obj, unsigned int size);

/* Garbage collection mark function type.  */
typedef int (*ulib_gcscan_func) (void *obj, void **ptr, unsigned int sz);

typedef struct ulib_cache ulib_cache;

/* Cache creation attributes.  */
#define ULIB_CACHE_SIZE   1
#define ULIB_CACHE_ALIGN  2
#define ULIB_CACHE_CTOR   3
#define ULIB_CACHE_CLEAR  4
#define ULIB_CACHE_DTOR   5
#define ULIB_CACHE_GC     6
#define ULIB_CACHE_GCSCAN 7

/* Create an object cache.  Throws NO_MEMORY, INVALID_PARAMETER.  */
ulib_cache *ulib_cache_create (int, ...);

/* Allocate an object from a cache.  Throws NO_MEMORY.  */
void *ulib_cache_alloc (ulib_cache *);

/* Release an object to the cache.  */
void ulib_cache_free (ulib_cache *, void *);

/* Release cached objects.  */
void ulib_cache_flush (ulib_cache *);

/* Register a non-cached root object.  */
int ulib_gcroot (void *, ulib_gcscan_func);

/* Register a cached root object.  */
int ulib_gcroot_cached (void *);

/* Unregister a root object.  */
void ulib_gcunroot (void *);

/* Push an allocation frame.  Objects, allocated in previous frames,
   won't be collected during the lifetime of this frame.  */
void ulib_gcpush (void);

/* Pop an allocation frame.  Live objects of the popped frame will be
   merged into the old frame.  */
void ulib_gcpop (void);

/* Perform garbage collection.  */
void ulib_gcrun (void);

END_DECLS

#endif /* ulib__cache_h */

/*
 * Local variables:
 * mode: C
 * indent-tabs-mode: nil
 * arch-tag: 9c1d93ec-d8f7-431d-bb9c-eafb7ac5f88f
 * End:
 */
