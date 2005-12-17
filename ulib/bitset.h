#ifndef ulib__bitset_h
#define ulib__bitset_h 1

#include "defs.h"
#include "vector.h"

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
int ulib_bitset_init (ulib_bitset *set);

/* Destroy a bit set.  */
void ulib_bitset_destroy (ulib_bitset *set);

/* Set each bit, whose index is in the array IDX of length N.  */
int ulib_bitset_set_bits (ulib_bitset *set, unsigned int n,
                          const unsigned int *idx);

/* Set bit N in the bitset SET.  */
int ulib_bitset_set (ulib_bitset *set, unsigned int n);

/* Set each bit, whose index is in the array IDX of length N.  */
int ulib_bitset_set_bits (ulib_bitset *set, unsigned int n,
                          const unsigned int *idx);

/* Clear bit N in the bitset SET.  */
void ulib_bitset_clear (ulib_bitset *set, unsigned int n);

/* Clear each bit, whose index is in the array IDX of length N.  */
void ulib_bitset_clear_bits (ulib_bitset *set, unsigned int n,
                             const unsigned int *idx);

/* Check whether bit N in the set SET is non-zero.  */
int ulib_bitset_is_set (const ulib_bitset *set, unsigned int n);

/* Set theoretic union: DST = SRC1 + SRC2.  */
int ulib_bitset_union (ulib_bitset *restrict dst,
                       const ulib_bitset *restrict src1,
                       const ulib_bitset *restrict src2);

/* In-place set theoretic union: DST += SRC.  */
int ulib_bitset_union_inplace (ulib_bitset *restrict dst,
                               const ulib_bitset *restrict src);

/* Set theoretic intersection: DST = SRC1 * SRC2.  */
int ulib_bitset_intersection (ulib_bitset *restrict dst,
                              const ulib_bitset *restrict src1,
                              const ulib_bitset *restrict src2);

/* In-place set theoretic intersection: DST *= SRC.  */
int ulib_bitset_intersection_inplace (ulib_bitset *restrict dst,
                                      const ulib_bitset *restrict src);

/* Set theoretic difference: DST = SRC1 - SRC2.  */
int ulib_bitset_difference (ulib_bitset *restrict dst,
                            const ulib_bitset *restrict src1,
                            const ulib_bitset *restrict src2);

/* In-place set theoretic difference: DST -= SRC.  */
int ulib_bitset_difference_inplace (ulib_bitset *restrict dst,
                                    const ulib_bitset *restrict src);

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
unsigned int ulib_bitset_max (const ulib_bitset *);

/* Check whether set A is a subset of set B.  */
int ulib_bitset_is_subset (const ulib_bitset *restrict a,
                           const ulib_bitset *restrict b);
END_DECLS
#endif /* ulib__bitset_h */

/*
 * Local variables:
 * mode: C
 * indent-tabs-mode: nil
 * arch-tag: f1497a9f-79e8-4504-963a-c2190d9f7172
 * End:
 */
