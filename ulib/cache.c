#include "cache.h"
#include "pgalloc.h"
#include "unwind.h"
#include "assert.h"
#include "alloc.h"
#include <string.h>
#include <stdarg.h>

#if HAVE_STDINT_H
#include <stdint.h>
#endif

#if !HAVE_UINTPTR_T
typedef unsigned long uintptr_t;
#endif

struct slab
{
  /* Doubly-linked lists of all the slabs in a cache.  */
  ulib_list list;

  /* The cache, to which this slab belongs.  */
  struct ulib_cache *cache;

  /* Beginning of object space.  */
  void *objects;

  /* Beginning of unallocated space.  */
  void *offset;

  /* Free objects list head.  */
  unsigned short free;

  /* Slab control bits: bit 15 - sweep flag, bits 0-14 - available
     objects count.  */
  unsigned short info;

  /* Object control bits: bit 15 - allocated, bit 14 - reachability,
     bits 0-13 - frame number when allocated, free list when
     available.  */
  unsigned short ctl [];
};

/* Object state constants.  */
#define GCFLAG     0x8000
#define ALLOCATED  0x4000
#define FRAME_MASK 0x1fff

/* End of list tag.  */
#define SLAB_EOL (0xffff & ~ALLOCATED)

/* Available objects count.  */
#define SLAB_COUNT(slab) (slab->info & ~GCFLAG)

/* The object cache structure.  */
struct ulib_cache
{
  /* Garbage collected caches list.  */
  ulib_list gclist;

  /* Head of list of all the slabs in the cache.  */
  ulib_list slabs;

  /* Pointer to the first non-empty slab in the cache.  */
  struct slab *free;

  /* Constructor function.  */
  ulib_ctor_func ctor;

  /* Clear function.  */
  ulib_clear_func clear;

  /* Destructor function.  */
  ulib_dtor_func dtor;

  /* GC scan function.  */
  ulib_gcscan_func scan;

  /* Next cache color.  */
  unsigned short color;

  /* Buffer size - includes padding to cache alignment.  */
  unsigned short size;

  /* Object size as given by the user.  */
  unsigned short usize;

  /* Alignment of objects.  */
  unsigned short align;

  /* Max count of objects in a slab.  */
  unsigned short object_count;

  /* Number of cache colors.  */
  unsigned short color_count;
};

/* Root objects tree.  */
struct root_data
{
  /* List of all registered root objects.  */
  ulib_list list;

  /* Root object scan function.  Having a separate scan function for
     root objects allows for non-cached roots.  */
  ulib_gcscan_func scan;

  /* Cached flag - if the root object is cached and thus
     garbage-collected, it is marked as reachable during the mark
     phase.  */
  unsigned int cached : 1;
};

#define ULIB_SPLAY_TREE_KEY_TYPE void *
#define ULIB_SPLAY_TREE_DATA_TYPE struct root_data
#define ULIB_SPLAY_TREE_COMPARE(a,b) ((char *) a - (char *) b)
#define ULIB_SPLAY_TREE_TYPE root_tree
#define ULIB_STATIC static

#include "splay-tree.h"
#include "splay-tree.c"

/* Root object constructor.  */
static void
root_tree_ctor (void *_obj, unsigned int size __attribute__ ((unused)))
{
  root_tree *obj = (root_tree *) _obj;

  obj->left = obj->right = 0;
  ulib_list_init (&obj->data.list);
}

/* "Globals".  */
static struct
{
  /* Garbage collected caches list.  */
  ulib_list gchead;

  /* Cache cache - allocator for cache objects.  */
  ulib_cache cache_cache;

  /* Roots cache - allocator for root registration objects.  */
  ulib_cache root_cache;

  /* The root of the tree of registered root objects.  */
  root_tree *roots;

  /* Reachability flag value.  The value alternates between 0 and
     GCFLAG before each mark phase, in order to avoid memory writes
     for clearing marks in live objects.  */
  unsigned short gcflag;

  /* Allocation frame number.  */
  unsigned short gcframe;
} G;

/* Align N to A boundary.  */
static inline unsigned int
align_uint (unsigned int n, unsigned int a)
{
  return (n + a - 1) & -a;
}

/* Align a pointer PTR to A boundary.  */
static inline void *
align_ptr (const void *ptr, unsigned int a)
{
  return (void *) (((uintptr_t) ptr + a - 1) & -a);
}

/* Calculate the number of objects in a slab.  */
static inline unsigned int
calc_nobjs (unsigned int size, unsigned int align)
{
  unsigned int nobjs, ctl;

  nobjs = (ulib_pgsize () - sizeof (struct slab)) / size + 1;
  do
    {
      nobjs--;
      ctl =
	align_uint (sizeof (struct slab) + nobjs * sizeof (short), align);
    }
  while (ctl + nobjs * size > ulib_pgsize ());

  return nobjs;
}

/* Calculate the color count for a slab.  */
static inline unsigned int
calc_ncolors (unsigned int count, unsigned int size, unsigned int align)
{
  return
    1 + ((ulib_pgsize ()
	  - align_uint (sizeof (struct slab) + count * sizeof (short),
			align)
	  - count * size)
	 / align);
}

/* Calculate slab object count and color count for the given object
   SIZE and ALIGNment.  Try to find parameters, which ensure both high
   space utilization and colorability.  */
static void
calc_slab_params (unsigned int size, unsigned int align,
		  unsigned short *object_count,
		  unsigned short *color_count)
{
  unsigned int nobjs, ncolors;

  nobjs = calc_nobjs (size, align) + 1;
  do
    {
      nobjs--;
      ncolors = calc_ncolors (nobjs, size, align);
    }
  while (ncolors == 1);

  *object_count = nobjs;
  *color_count = ncolors;
}

/* Initialize a cache.  */
static void
cache_init (ulib_cache  *cache, unsigned int size, unsigned int align,
	    ulib_ctor_func ctor, ulib_clear_func clear, ulib_dtor_func dtor,
	    int gc, ulib_gcscan_func scan)
{
  ulib_list_init (&cache->gclist);
  if (gc)
    ulib_list_insert (&G.gchead, &cache->gclist);
  ulib_list_init (&cache->slabs);
  cache->free = (struct slab *) &cache->slabs.next;
  cache->ctor = ctor;
  cache->clear = clear;
  cache->dtor = dtor;
  cache->scan = scan;
  cache->color = 0;
  cache->size = align_uint (size, align);
  cache->usize = size;
  cache->align = align;
  calc_slab_params (cache->size, align, &cache->object_count,
		    &cache->color_count);
}

/* PRIVATE: Initialize the cacheing allocator.  */
static int cache_initialized;

static void
init_cache ()
{
  cache_initialized = 1;
  ulib_list_init (&G.gchead);
  cache_init (&G.cache_cache, sizeof (ulib_cache),
	      sizeof (void *), 0, 0, 0, 0, 0);
  cache_init (&G.root_cache, sizeof (root_tree), sizeof (void *),
	      root_tree_ctor, 0, 0, 0, 0);
  G.gcframe = 0;
}

/* Create a cache.  */
ulib_cache *
ulib_cache_create (int attr, ...)
{
  va_list ap;
  ulib_cache *cache;
  int gc = 0;
  unsigned int size = 0, align = 0;
  ulib_ctor_func ctor = 0;
  ulib_clear_func clear = 0;
  ulib_dtor_func dtor = 0;
  ulib_gcscan_func scan = 0;

  if (!cache_initialized)
    init_cache ();

  va_start (ap, attr);
  do
    {
      switch (attr)
	{
	case ULIB_CACHE_SIZE:
	  size = va_arg (ap, unsigned int);
	  break;

	case ULIB_CACHE_ALIGN:
	  align = va_arg (ap, unsigned int);
	  break;

	case ULIB_CACHE_CTOR:
	  ctor = va_arg (ap, ulib_ctor_func);
	  break;

	case ULIB_CACHE_CLEAR:
	  clear = va_arg (ap, ulib_clear_func);
	  break;

	case ULIB_CACHE_DTOR:
	  dtor = va_arg (ap, ulib_dtor_func);
	  break;

	case ULIB_CACHE_GC:
	  gc = 1;
	  break;

	case ULIB_CACHE_GCSCAN:
	  scan = va_arg (ap, ulib_gcscan_func);
	  break;

	case 0:
	  break;

	default:
	  ulib_unwind_throw_ptr (ULIB_INVALID_PARAMETER,
				 "Invalid cache attribute");
	  return 0;
	}
      attr = va_arg (ap, unsigned int);
    }
  while (attr);
  va_end (ap);

  if (scan)
    gc = 1;

  if (size < ULIB_CACHE_OBJECT_SIZE_MIN)
    size = ULIB_CACHE_OBJECT_SIZE_MIN;

  if (align < ULIB_CACHE_OBJECT_ALIGN_MIN)
    size = ULIB_CACHE_OBJECT_ALIGN_MIN;

  if (size > ULIB_CACHE_OBJECT_SIZE_MAX
      || align > ULIB_CACHE_OBJECT_ALIGN_MAX)
    ulib_unwind_throw_ptr (ULIB_INVALID_PARAMETER,
			   "Invalid cache attribute value");

  cache = ulib_cache_alloc (&G.cache_cache);
  cache_init (cache, size, align, ctor, clear, dtor, gc, scan);

  return cache;
}

/* Initialize and add a slab to a cache.  */
static struct slab *
slab_init (ulib_cache *cache, char *ptr)
{
  struct slab *slab;

  /* Initialize a slab structure at the beginning of the page.  */
  slab = (struct slab *) ptr;
  ptr += sizeof (struct slab);

  ulib_list_init (&slab->list);
  slab->cache = cache;
  slab->free = SLAB_EOL;
  slab->info = G.gcflag | cache->object_count;

  /* Clear object status bits.  */
  memset (ptr, 0, cache->object_count * sizeof (short));
  ptr += cache->object_count * sizeof (short);

  /* Set the beginning of the object array depending on the next cache
     color.  */
  ptr = align_ptr (ptr, cache->align);
  slab->objects = slab->offset = ptr + cache->color * cache->align;

  /* Set the color number for the next slab.  */
  cache->color = (cache->color + 1) % cache->color_count;

  /* Put the slab on the cache lists.  */
  ulib_list_insert (&cache->slabs, &slab->list);
  if (cache->free == (struct slab *) &cache->slabs.next)
    cache->free = slab;

  return slab;
}

/* Find the slab, to which the object PTR belongs.  */
static inline struct slab *
object_slab (const void *ptr)
{
  return (struct slab *) ((uintptr_t) ptr & -ulib_pgsize ());
}

/* Return the index of the object PTR, which belongs to SLAB.  */
static inline unsigned short
object_index (const struct slab *slab, const void *ptr)
{
  return ((char *) ptr - (char *) slab->objects) / slab->cache->size;
}

/* Allocate an object from a slab cache.  */
void *
ulib_cache_alloc (ulib_cache *cache)
{
  char *ptr;
  int run_ctor;
  struct slab *slab;
  unsigned short index;

  /* Get a non-empty slab.  Allocate one, if needed.  */
  slab = cache->free;
  if (slab == (struct slab *) &cache->slabs)
    {
      ptr = (char *) ulib_pgalloc ();
      slab = slab_init (cache, ptr);
    }

  /* Allocate an object - either from the slab's free list or from the
     unused space near the end of the slab.  */
  if ((index = slab->free) != SLAB_EOL)
    {
      ptr = (char *) slab->objects + index * cache->size;
      slab->free = slab->ctl [index];
      run_ctor = 0;
    }
  else
    {
      index = cache->object_count - SLAB_COUNT (slab);
      ptr = slab->offset;
      slab->offset = ptr + cache->size;
      run_ctor = 1;
    }

  /* Mark the object as allocated, unreachable and record allocation
     frame number.  */
  slab->ctl [index] = G.gcframe | G.gcflag | ALLOCATED;

  /* Decrement the available objects count.  If the slab became empty,
     advance the cache free list pointer to the next slab.  */
  slab->info--;
  if (SLAB_COUNT (slab) == 0)
    cache->free = (struct slab *) slab->list.next;

  /* Run the constructor if there is one.  */
  if (run_ctor && cache->ctor)
    cache->ctor (ptr, cache->usize);

  return ptr;
}

/* Release an object to the cache.  */
void
ulib_cache_free (ulib_cache *cache, void *ptr)
{
  struct slab *slab;
  unsigned short index;

  /* Clear the object.  */
  slab = object_slab (ptr);
  ulib_assert (cache == slab->cache);
  if (cache->clear)
    cache->clear (ptr, cache->usize);

  /* Put the object in front of the slab free list.  This clears the
     ALLOCATED flag, too.  Increment the slab objects count.  */
  index = object_index (slab, ptr);
  slab->ctl [index] = slab->free;
  slab->free = index;

  /* Increment objects count.  If the slab becomes non-empty, put it
     at the beginning of the cache's free list.  */
  slab->info++;
  if (SLAB_COUNT (slab) == 1)
    {
      ulib_list_remove (&slab->list);
      ulib_list_append (cache->free, &slab->list);
    }

  /* If the slab becomes full, move it at the end of both the cache's
     free list and the all slabs list.  */
  else if (SLAB_COUNT (slab) == cache->object_count)
    {
      if (cache->free == slab)
	cache->free = (struct slab *) slab->list.next;
      ulib_list_remove (&slab->list);

      ulib_list_insert (&cache->slabs, &slab->list);
      if (cache->free == (struct slab *) &cache->slabs)
	cache->free = slab;
    }
}

/* Release cached objects in CACHE.  Walk over full slabs, run the
   destructors of objects, cached there and release the slab's page.
   Full slabs are positioned at the end of the cache's slab list and
   are not intermixed with (partially) empty slabs.  */
void
ulib_cache_flush (ulib_cache *cache)
{
  struct slab *slab, *prev;
  unsigned short index;
  void *obj;

  slab = (struct slab *) cache->slabs.prev;
  while (slab != (struct slab *) &cache->slabs
	 && SLAB_COUNT (slab) == cache->object_count)
    {
      prev = (struct slab *) slab->list.prev;
      if (cache->free == slab)
	cache->free = (struct slab *) slab->list.next;

      index = slab->free;
      while (index != SLAB_EOL)
	{
	  obj = (char *) slab->objects + index * cache->size;
	  if (cache->dtor)
	    cache->dtor (obj, cache->usize);
	  index = slab->ctl [index];
	}

      ulib_list_remove (&slab->list);
      ulib_pgfree (slab);

      slab = prev;
    }
}

/* Helper function to allocate and register a root object.  */
static root_tree *
gcroot (void *obj)
{
  int root_already_registered;
  root_tree *root;

  if (!cache_initialized)
    init_cache ();

  root = ulib_cache_alloc (&G.root_cache);
  root->key = obj;

  if (G.roots)
    ulib_list_insert (&G.roots->data.list, &root->data.list);

  root_already_registered = root_tree_insert (&G.roots, root);
  ulib_assert (root_already_registered == 0);

  return root;
}

/* Register a non-cached root object.  */
void
ulib_gcroot (void *obj, ulib_gcscan_func scan)
{
  root_tree *root;

  root = gcroot (obj);
  root->data.scan = scan;
  root->data.cached = 0;
}

/* Register a cached root object.  */
void
ulib_gcroot_cached (void *obj)
{
  root_tree *root;

  root = gcroot (obj);
  root->data.scan = object_slab (obj)->cache->scan;
  root->data.cached = 1;
}

/* Unregister a root object.  */
void
ulib_gcunroot (void *ptr)
{
  root_tree *root;

  root = root_tree_delete (&G.roots, ptr);
  ulib_assert (root != 0);

  ulib_list_remove (&root->data);
  ulib_cache_free (&G.root_cache, root);
}

/* Can't portably use nested functions ... *sigh* ... */
struct gc_mark_frame
{
  void **objs;
  unsigned int n;
  unsigned int sz;
};

/* Scan OBJ (by invoking the SCAN function) for references to garbage
   collected objects.  Record immediately reachanble objects in the
   array FRM->OBJS.  The parameter FRM->N is the number of elements in
   the array, while the parameter FRM->SZ is the allocated size of the
   array.  Set the new number of elements in FRM->N.  */
static void
gc_scan_obj (ulib_gcscan_func scan, void *obj,
	     struct gc_mark_frame *frm)
{
  int cnt;

  cnt = scan (obj, frm->objs + frm->n, frm->sz - frm->n);
  if (cnt < 0)
    {
      frm->sz -= cnt;
      frm->objs = ulib_realloc (frm->objs, frm->sz * sizeof (void *));
      cnt = scan (obj, frm->objs + frm->n, frm->sz - frm->n);
      ulib_assert (cnt > 0);
    }
  frm->n += cnt;
}

/* Perform the mark phase of the collector.  The mark process starts
   at each registered root and proceeds in depth first search order
   over the objects interreference graph.  An object is scanned iff it
   is allocated and not visited yet, i.e. its reachability status
   differs from the current reachability flag.  The array OBJS is used
   in a stack-like fashion to keep track of the objects pending
   scanning.  */
static void
gc_mark ()
{
  void *obj;
  struct slab *slab;
  unsigned short index;
  struct gc_mark_frame frm;
  root_tree *head, *root;

  if ((head = G.roots) == 0)
    return;

  frm.objs = 0;
  frm.sz = 0;
  root = head;
  do
    {
      frm.n = 0;
      /* Scan the root object.  */
      if (root->data.cached == 0)
	gc_scan_obj (root->data.scan, root->key, &frm);
      else
	{
	  slab = object_slab (root->key);
	  index = object_index (slab, root->key);
	  ulib_assert (slab->ctl [index] & ALLOCATED);

	  if ((slab->ctl [index] & GCFLAG) != G.gcflag)
	    {
	      slab->ctl [index] ^= GCFLAG;
	      gc_scan_obj (root->data.scan, root->key, &frm);
	    }
	}

      /* Scan pending objects.  */
      while (frm.n--)
	{
	  obj = frm.objs [frm.n];
	  slab = object_slab (obj);
	  index = object_index (slab, obj);

	  if ((slab->ctl [index] & ALLOCATED)
	      && (slab->ctl [index] & GCFLAG) != G.gcflag)
	    {
	      slab->ctl [index] ^= GCFLAG;
	      if (slab->cache->scan)
		gc_scan_obj (slab->cache->scan, obj, &frm);
	    }
	}

      root = (root_tree *)
	((char *) root->data.list.next - offsetof (root_tree, data));
    }
  while (root != head);

  ulib_free (frm.objs);
}

/* Perform the sweep phase of the collector.  Traverse the allocated
   objects in each slab of each garbage collected cache.  Free each
   object with clear reachability flag and frame number equal to the
   current one.  Toggle the reachability flag of the rest of the
   objects, so it remains consistent for the next mark phase.  If the
   MERGE parameter is true, decrement the frame number of each
   reachable object, which belongs to the current frame, effectively
   merging the current allocation frame into the previous one.  */
static void
gc_sweep (int merge)
{
  void *obj;
  struct slab *slab, *next;
  ulib_cache *cache;
  unsigned int n;
  unsigned short ctl;

  for (cache = (ulib_cache *) G.gchead.next;
       cache != (ulib_cache *) &G.gchead;
       cache = (ulib_cache *) cache->gclist.next)
    {
      slab = (struct slab *) cache->slabs.next;
      while (slab != (struct slab *) &cache->slabs)
	{
	  next = (struct slab *) slab->list.next;

	  n = cache->object_count;
	  if (SLAB_COUNT (slab) < n && (slab->info & GCFLAG) != G.gcflag)
	    {
	      slab->info ^= GCFLAG;

	      obj = (char *) slab->objects + n * cache->size;
	      while (n--)
		{
		  ctl = slab->ctl [n];
		  obj = (char *) obj - cache->size;

		  if ((ctl & ALLOCATED) == 0)
		    continue;

		  if ((ctl & GCFLAG) != G.gcflag)
		    slab->ctl [n] ^= GCFLAG;

		  if ((ctl & FRAME_MASK) != G.gcframe)
		    continue;

		  if ((ctl & GCFLAG) != G.gcflag)
		    ulib_cache_free (cache, obj);
		  else if (merge)
		    slab->ctl [n] = (G.gcframe - 1) | G.gcflag | ALLOCATED;
		}
	    }

	  slab = next;
	}
      ulib_cache_flush (cache);
    }
}

/* Push an allocation frame.  Objects, allocated in previous frames,
   won't be collected during the lifetime of this frame.  */
void
ulib_gcpush ()
{
  if (!cache_initialized)
    init_cache ();
  G.gcframe++;
}

/* Pop an allocation frame.  Live objects of the popped frame will be
   merged into the old frame.  */
void
ulib_gcpop ()
{
  if (!cache_initialized)
    init_cache ();
  G.gcflag ^= GCFLAG;
  gc_mark ();
  gc_sweep (1);
  G.gcframe--;
}

/* Perform garbage collection.  */
void ulib_gcrun ()
{
  if (!cache_initialized)
    init_cache ();
  G.gcflag ^= GCFLAG;
  gc_mark ();
  gc_sweep (0);
}

/*
 * Local variables:
 * mode: C
 * indent-tabs-mode: nil
 * arch-tag: a6b54383-e52c-4718-900f-aa1586ee527d
 * End:
 */
