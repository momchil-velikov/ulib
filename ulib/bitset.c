#include "bitset.h"
#include <limits.h>

/* Bitset vector element type.  */
#if ULONG_MAX > 4294967295UL
typedef unsigned long bitset_elt;
#else
typedef unsigned int bitset_elt;
#endif

/* Element index of bit number N.  */
static inline unsigned int
elt_index (unsigned int n)
{
  return n / (sizeof (bitset_elt) * CHAR_BIT);
}

/* Bit index within an element of bit number N.  */
static inline unsigned int
bit_index (unsigned int n)
{
  return n % (sizeof (bitset_elt) * CHAR_BIT);
}

/* Maximum of two unsigned integers. */
static inline unsigned int
max_uint (unsigned int a, unsigned int b)
{
  return a > b ? a : b;
}

/* Minimum of two unsigned integers. */
static inline unsigned int
min_uint (unsigned int a, unsigned int b)
{
  return a < b ? a : b;
}

/* The index of the first set bit, from MSB to LSB.  */
static const unsigned int msb_set_tab [] =
{
  /* 0000 */ 0,
  /* 0001 */ 4,
  /* 0010 */ 3,
  /* 0011 */ 3,
  /* 0100 */ 2,
  /* 0101 */ 2,
  /* 0110 */ 2,
  /* 0111 */ 2,
  /* 1000 */ 1,
  /* 1001 */ 1,
  /* 1010 */ 1,
  /* 1011 */ 1,
  /* 1100 */ 1,
  /* 1101 */ 1,
  /* 1110 */ 1,
  /* 1111 */ 1
};

unsigned int
bitset_elt_ffs (bitset_elt elt)
{
  bitset_elt mask;

  unsigned int k, n;

  n = (sizeof (bitset_elt) * CHAR_BIT) >> 1;
  mask = (((bitset_elt) 1 << n) - 1) << n;

  k = 0;
  while (n >= 4)
    {
      if (elt & mask)
        elt >>= n;
      else
        k += n;
      n >>= 1;
      mask = (mask & (mask >> n)) >> n;
    }

  return sizeof (bitset_elt) * CHAR_BIT - (k + msb_set_tab [elt]);
}

/* Initialize a bit set.  */
int
ulib_bitset_init (ulib_bitset *set)
{
  return ulib_vector_init (&set->bits, ULIB_ELT_SIZE, sizeof (bitset_elt), 0);
}

/* Destroy a bit set.  */
void
ulib_bitset_destroy (ulib_bitset *set)
{
  ulib_vector_destroy (&set->bits);
}

/* Set bit N in the bitset SET.  */
int
ulib_bitset_set (ulib_bitset *set, unsigned int n)
{
  bitset_elt *elt;

  if (ulib_vector_atleast (&set->bits, elt_index (n) + 1) < 0)
    return -1;

  elt = (bitset_elt *) ulib_vector_elt (&set->bits, elt_index (n));
  *elt |= 1U << bit_index (n);

  return 0;
}

/* Clear bit N in the bitset SET.  */
void
ulib_bitset_clear (ulib_bitset *set, unsigned int n)
{
  if (elt_index (n) < ulib_vector_length (&set->bits))
    {
      bitset_elt *elt;

      elt = (bitset_elt *) ulib_vector_elt (&set->bits, elt_index (n));
      *elt &= ~(1U << bit_index (n));
    }
}

/* Check whether bit N in the set SET is non-zero.  */
int
ulib_bitset_isset (const ulib_bitset *set, unsigned int n)
{
  if (elt_index (n) >= ulib_vector_length (&set->bits))
    return 0;
  else
    {
      bitset_elt *elt;

      elt = (bitset_elt *) ulib_vector_elt (&set->bits, elt_index (n));

      return *elt & (1U << bit_index (n));
    }
}

/* Set theoretic union: DST = SRC1 + SRC2.  */
int
ulib_bitset_union (ulib_bitset *dst, const ulib_bitset *src1,
                   const ulib_bitset *src2)
{
  unsigned int i, mn, mx;
  bitset_elt *d, *s1, *s2;

  mx = max_uint (ulib_vector_length (&src1->bits),
                 ulib_vector_length (&src2->bits));

  if (ulib_vector_set_size (&dst->bits, mx) < 0)
    return -1;

  if (mx == 0)
    return 0;

  mn = min_uint (ulib_vector_length (&src1->bits),
                 ulib_vector_length (&src2->bits));

  d = (bitset_elt *) ulib_vector_elt (&dst->bits, 0);
  s1 = (bitset_elt *) ulib_vector_elt (&src1->bits, 0);
  s2 = (bitset_elt *) ulib_vector_elt (&src2->bits, 0);

  for (i = 0; i < mn; ++i)
    *d++ = *s1++ | *s2++;

  if (i < ulib_vector_length (&src2->bits))
    s1 = s2;

  for (; i < mx; ++i)
    *d++ = *s1++;

  return 0;
}

/* In-place set theoretic union: DST += SRC.  */
int
ulib_bitset_union_inplace (ulib_bitset *dst, const ulib_bitset *src)
{
  unsigned int i, mn, mx;
  bitset_elt *d, *s;

  mx = max_uint (ulib_vector_length (&dst->bits),
                 ulib_vector_length (&src->bits));

  if (mx == 0)
    return 0;

  if (ulib_vector_set_size (&dst->bits, mx) < 0)
    return -1;

  mn = min_uint (ulib_vector_length (&dst->bits),
                 ulib_vector_length (&src->bits));

  d = (bitset_elt *) ulib_vector_elt (&dst->bits, 0);
  s = (bitset_elt *) ulib_vector_elt (&src->bits, 0);

  for (i = 0; i < mn; ++i)
    *d++ |= *s++;

  if (i < ulib_vector_length (&src->bits))
    {
      for (; i < mx; ++i)
        *d++ = *s++;
    }

  return 0;
}

/* Set theoretic intersection: DST = SRC1 * SRC2.  */
int
ulib_bitset_intersection (ulib_bitset *dst, const ulib_bitset *src1,
                          const ulib_bitset *src2)
{
  unsigned int i, mn;
  bitset_elt *d, *s1, *s2;

  mn = min_uint (ulib_vector_length (&src1->bits),
                 ulib_vector_length (&src2->bits));

  if (ulib_vector_set_size (&dst->bits, mn) < 0)
    return -1;

  if (mn == 0)
    return 0;

  d = (bitset_elt *) ulib_vector_elt (&dst->bits, 0);
  s1 = (bitset_elt *) ulib_vector_elt (&src1->bits, 0);
  s2 = (bitset_elt *) ulib_vector_elt (&src2->bits, 0);

  for (i = 0; i < mn; ++i)
    *d++ = *s1++ & *s2++;

  return 0;
}

/* In-place set theoretic intersection: DST *= SRC.  */
int
ulib_bitset_intersection_inplace (ulib_bitset *dst, const ulib_bitset *src)
{
  unsigned int i, mn;
  bitset_elt *d, *s;

  mn = min_uint (ulib_vector_length (&dst->bits),
                 ulib_vector_length (&src->bits));

  if (ulib_vector_set_size (&dst->bits, mn) < 0)
    return -1;

  if (mn == 0)
    return 0;

  d = (bitset_elt *) ulib_vector_elt (&dst->bits, 0);
  s = (bitset_elt *) ulib_vector_elt (&src->bits, 0);

  for (i = 0; i < mn; ++i)
    *d++ &= *s++;

  return 0;
}

/* Set theoretic difference: DST = SRC1 - SRC2.  */
int
ulib_bitset_difference (ulib_bitset *dst, const ulib_bitset *src1,
                        const ulib_bitset *src2)
{
  unsigned int i, mn;
  bitset_elt *d, *s1, *s2;

  if (ulib_vector_set_size (&dst->bits, ulib_vector_length (&src1->bits)) < 0)
    return -1;

  mn = min_uint (ulib_vector_length (&src1->bits),
                 ulib_vector_length (&src2->bits));
  if (mn == 0)
    return 0;

  d = (bitset_elt *) ulib_vector_elt (&dst->bits, 0);
  s1 = (bitset_elt *) ulib_vector_elt (&src1->bits, 0);
  s2 = (bitset_elt *) ulib_vector_elt (&src2->bits, 0);

  for (i = 0; i < mn; ++i)
    *d++ = *s1++ & ~ *s2++;

  return 0;
}

/* In-place set theoretic difference: DST -= SRC.  */
int
ulib_bitset_difference_inplace (ulib_bitset *dst, const ulib_bitset *src)
{
  unsigned int i, mn;
  bitset_elt *d, *s;

  mn = min_uint (ulib_vector_length (&dst->bits),
                 ulib_vector_length (&src->bits));
  if (mn == 0)
    return 0;

  d = (bitset_elt *) ulib_vector_elt (&dst->bits, 0);
  s = (bitset_elt *) ulib_vector_elt (&src->bits, 0);

  for (i = 0; i < mn; ++i)
    *d++ &= ~ *s++;

  return 0;
}

/* Return one plus the index of the last bit set.  */
unsigned int
ulib_bitset_max (const ulib_bitset *set)
{
  unsigned int i;
  bitset_elt *elt;

  if ((i = ulib_vector_length (&set->bits)) == 0)
    return 0;

  elt = ulib_vector_back (&set->bits);
  while (i--)
    {
      if (*--elt)
        return i * sizeof (bitset_elt) * CHAR_BIT + bitset_elt_ffs (*elt) + 1;
    }

  return 0;
}

/*
 * Local variables:
 * mode: C
 * indent-tabs-mode: nil
 * arch-tag: 8fab10b8-526e-4130-a295-cb6d120ad437
 * End:
 */
