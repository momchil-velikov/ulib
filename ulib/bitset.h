#ifndef ulib__bitset_h
#define ulib__bitset_h 1

#include "defs.h"
#include "vector.h"
#include "ulib-if.h"

BEGIN_DECLS

struct ulib_bitset
{
  /* Bit vector.  */
  ulib_vector bits;

  /* Index of the last used element.  */
  unsigned int used_len;
};
typedef struct ulib_bitset ulib_bitset;

/* Initialize an empty bit set.  */
ULIB_IF int ulib_bitset_init (ulib_bitset *set);

/* Destroy a bit set.  */
ULIB_IF void ulib_bitset_destroy (ulib_bitset *set);

/* Set each bit, whose index is in the array IDX of length N.  */
ULIB_IF int ulib_bitset_set_bits (ulib_bitset *set, unsigned int n,
                                  const unsigned int *idx);

/* Set bit N in the bitset SET.  */
ULIB_IF int ulib_bitset_set (ulib_bitset *set, unsigned int n);

/* Set each bit, whose index is in the array IDX of length N.  */
ULIB_IF int ulib_bitset_set_bits (ulib_bitset *set, unsigned int n,
                                  const unsigned int *idx);

/* Clear bit N in the bitset SET.  */
ULIB_IF void ulib_bitset_clear (ulib_bitset *set, unsigned int n);

/* Clear each bit, whose index is in the array IDX of length N.  */
ULIB_IF void ulib_bitset_clear_bits (ulib_bitset *set, unsigned int n,
                                     const unsigned int *idx);

/* Clear all bits in a bitset.  */
static inline void
ulib_bitset_clear_all (ulib_bitset *set)
{
  set->used_len = 0;
}

/* Check whether bit N in the set SET is non-zero.  */
ULIB_IF int ulib_bitset_is_set (const ulib_bitset *set, unsigned int n);

/* Set theoretic union: DST = SRC1 | SRC2.  */
ULIB_IF int ulib_bitset_or (ulib_bitset *restrict dst,
                            const ulib_bitset *restrict src1,
                            const ulib_bitset *restrict src2);

/* Destructive set theoretic union: DST |= SRC.  */
ULIB_IF int ulib_bitset_destr_or (ulib_bitset *restrict dst,
                                  const ulib_bitset *restrict src);

/* Destructive set theoretic union: DST |= SRC.  Returns negative on
   error, positive if the DST changed or zero otherwise.  */
ULIB_IF int ulib_bitset_destr_or_chg (ulib_bitset *restrict dst,
                                      const ulib_bitset *restrict src);

/* Set theoretic intersection: DST = SRC1 & SRC2.  */
ULIB_IF int ulib_bitset_and (ulib_bitset *restrict dst,
                             const ulib_bitset *restrict src1,
                             const ulib_bitset *restrict src2);

/* Destructive set theoretic intersection: DST &= SRC.  */
ULIB_IF int ulib_bitset_destr_and (ulib_bitset *restrict dst,
                                   const ulib_bitset *restrict src);

/* Destructive set theoretic intersection: DST &= SRC.  Returns
   negative on error, positive if the DST changed or zero
   otherwise. */
ULIB_IF int ulib_bitset_destr_and_chg (ulib_bitset *restrict dst,
                                       const ulib_bitset *restrict src);

/* Set theoretic difference: DST = SRC1 & ~SRC2.  */
ULIB_IF int ulib_bitset_andn (ulib_bitset *restrict dst,
                              const ulib_bitset *restrict src1,
                              const ulib_bitset *restrict src2);

/* Destructive set theoretic difference: DST &= ~SRC.  */
ULIB_IF int ulib_bitset_destr_andn (ulib_bitset *restrict dst,
                                    const ulib_bitset *restrict src);

/* Destructive set theoretic difference: DST = & ~SRC.  Returns
   negative on error, positive if the DST changed or zero
   otherwise.  */
ULIB_IF int ulib_bitset_destr_andn_chg (ulib_bitset *restrict dst,
                                        const ulib_bitset *restrict src);

/* Compute DST = SRC1 | (SRC2 & ~SRC3)  */
ULIB_IF int ulib_bitset_or_andn (ulib_bitset *restrict dst,
                                 const ulib_bitset *restrict src1,
                                 const ulib_bitset *restrict src2,
                                 const ulib_bitset *restrict src3);

/* Compute DST |= SRC1 & ~SRC2  */
ULIB_IF int ulib_bitset_destr_or_andn (ulib_bitset *restrict dst,
                                       const ulib_bitset *restrict src1,
                                       const ulib_bitset *restrict src2);

/* Compute DST |= SRC1 & ~SRC2.  Returns negative on error, positive
   if the DST changed or zero otherwise.  */
ULIB_IF int ulib_bitset_destr_or_andn_chg (ulib_bitset *restrict dst,
                                           const ulib_bitset *restrict src1,
                                           const ulib_bitset *restrict src2);
/* Copy a bitset.  */
static inline int
ulib_bitset_copy (ulib_bitset *restrict dst, const ulib_bitset *restrict src)
{
  if (ulib_vector_copy (&dst->bits, &src->bits) < 0)
    return -1;

  dst->used_len = src->used_len;
  return 0;
}

/* Return the one plus the index of the last bit set.  */
ULIB_IF unsigned int ulib_bitset_max (const ulib_bitset *);

/* Check whether set A is a subset of set B.  */
ULIB_IF int ulib_bitset_is_subset (const ulib_bitset *restrict a,
                                   const ulib_bitset *restrict b);

/* Check whether a bitset is empty.  */
static inline int
ulib_bitset_is_empty (const ulib_bitset *set)
{
  return set->used_len == 0;
}

END_DECLS
#endif /* ulib__bitset_h */

/*
 * Local variables:
 * mode: C
 * indent-tabs-mode: nil
 * End:
 */
