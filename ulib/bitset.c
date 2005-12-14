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

/* Return one plus the index of the last used array element.  */
static unsigned int
used_len (const ulib_bitset *set)
{
  unsigned int i;
  bitset_elt *elt;

  if (set->used_len == 0)
    return 0;

  i = set->used_len - 1;
  elt = ulib_vector_elt (&set->bits, i);
  while (*elt == 0 && i-- > 0)
    --elt;

  return i + 1;
}

/* Initialize a bit set.  */
int
ulib_bitset_init (ulib_bitset *set)
{
  set->used_len = 0;
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
  unsigned int idx = elt_index (n);

  if (ulib_vector_atleast (&set->bits, idx + 1) < 0)
    return -1;

  elt = (bitset_elt *) ulib_vector_elt (&set->bits, idx);
  *elt |= 1U << bit_index (n);

  if (set->used_len <= idx)
    set->used_len = idx + 1;

  return 0;
}

/* Clear bit N in the bitset SET.  */
void
ulib_bitset_clear (ulib_bitset *set, unsigned int n)
{
  unsigned int idx = elt_index (n);

  if (idx < ulib_vector_length (&set->bits))
    {
      bitset_elt *elt;

      elt = (bitset_elt *) ulib_vector_front (&set->bits);
      elt [idx] &= ~(1U << bit_index (n));

      if (idx + 1 == set->used_len)
        set->used_len = used_len (set);
    }
}

/* Check whether bit N in the set SET is non-zero.  */
int
ulib_bitset_isset (const ulib_bitset *set, unsigned int n)
{
  unsigned int idx = elt_index (n);

  if (idx >= ulib_vector_length (&set->bits))
    return 0;
  else
    {
      bitset_elt *elt;

      elt = (bitset_elt *) ulib_vector_elt (&set->bits, idx);

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

  mx = max_uint (src1->used_len, src2->used_len);
  
  if (ulib_vector_set_size (&dst->bits, mx) < 0)
    return -1;
  dst->used_len = mx;
  if (mx == 0)
    return 0;

  mn = min_uint (src1->used_len, src2->used_len);

  d = (bitset_elt *) ulib_vector_front (&dst->bits);
  s1 = (bitset_elt *) ulib_vector_front (&src1->bits);
  s2 = (bitset_elt *) ulib_vector_front (&src2->bits);

  for (i = 0; i < mn; ++i)
    *d++ = *s1++ | *s2++;

  if (i < src2->used_len)
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

  if ((mx = max_uint (dst->used_len, src->used_len)) == 0)
    return 0;

  if (ulib_vector_set_size (&dst->bits, mx) < 0)
    return -1;

  mn = min_uint (dst->used_len, src->used_len);
  d = (bitset_elt *) ulib_vector_front (&dst->bits);
  s = (bitset_elt *) ulib_vector_front (&src->bits);

  for (i = 0; i < mn; ++i)
    *d++ |= *s++;

  if (i < src->used_len)
    {
      for (; i < mx; ++i)
        *d++ = *s++;
    }
  dst->used_len = mx;

  return 0;
}

/* Set theoretic intersection: DST = SRC1 * SRC2.  */
int
ulib_bitset_intersection (ulib_bitset *dst, const ulib_bitset *src1,
                          const ulib_bitset *src2)
{
  unsigned int i, mn;
  bitset_elt *d, *s1, *s2;

  mn = min_uint (src1->used_len, src2->used_len);

  if (ulib_vector_set_size (&dst->bits, mn) < 0)
    return -1;
  dst->used_len = mn;
  if (mn == 0)
    return 0;

  d = (bitset_elt *) ulib_vector_front (&dst->bits);
  s1 = (bitset_elt *) ulib_vector_front (&src1->bits);
  s2 = (bitset_elt *) ulib_vector_front (&src2->bits);

  for (i = 0; i < mn; ++i)
    *d++ = *s1++ & *s2++;

  if (d [-1] == 0)
    dst->used_len = used_len (dst);

  return 0;
}

/* In-place set theoretic intersection: DST *= SRC.  */
int
ulib_bitset_intersection_inplace (ulib_bitset *dst, const ulib_bitset *src)
{
  unsigned int i, mn;
  bitset_elt *d, *s;

  mn = min_uint (dst->used_len, src->used_len);

  if (ulib_vector_set_size (&dst->bits, mn) < 0)
    return -1;
  dst->used_len = mn;
  if (mn == 0)
    return 0;

  d = (bitset_elt *) ulib_vector_front (&dst->bits);
  s = (bitset_elt *) ulib_vector_front (&src->bits);

  for (i = 0; i < mn; ++i)
    *d++ &= *s++;

  if (d [-1] == 0)
    dst->used_len = used_len (dst);

  return 0;
}

/* Set theoretic difference: DST = SRC1 - SRC2.  */
int
ulib_bitset_difference (ulib_bitset *dst, const ulib_bitset *src1,
                        const ulib_bitset *src2)
{
  unsigned int i, mn;
  bitset_elt *d, *s1, *s2;

  if (ulib_vector_set_size (&dst->bits, src1->used_len))
    return -1;
  dst->used_len = src1->used_len;
  if (dst->used_len == 0)
    return 0;

  mn = min_uint (src1->used_len, src2->used_len);

  d = (bitset_elt *) ulib_vector_front (&dst->bits);
  s1 = (bitset_elt *) ulib_vector_front (&src1->bits);
  s2 = (bitset_elt *) ulib_vector_front (&src2->bits);

  for (i = 0; i < mn; ++i)
    *d++ = *s1++ & ~ *s2++;

  if (i < src1->used_len)
    {
      for (; i < src1->used_len; ++i)
        *d++ = *s1++;
    }

  if (d [-1] == 0)
    dst->used_len = used_len (dst);

  return 0;
}

/* In-place set theoretic difference: DST -= SRC.  */
int
ulib_bitset_difference_inplace (ulib_bitset *dst, const ulib_bitset *src)
{
  unsigned int i, mn;
  bitset_elt *d, *s;

  mn = min_uint (dst->used_len, src->used_len);
  if (mn == 0)
    return 0;

  d = (bitset_elt *) ulib_vector_front (&dst->bits);
  s = (bitset_elt *) ulib_vector_front (&src->bits);

  for (i = 0; i < mn; ++i)
    *d++ &= ~ *s++;

  if (d [-1] == 0)
    dst->used_len = used_len (dst);

  return 0;
}

/* Return one plus the index of the last bit set.  */
unsigned int
ulib_bitset_max (const ulib_bitset *set)
{
  bitset_elt *elt;

  if (set->used_len == 0)
    return 0;

  elt = ulib_vector_elt (&set->bits, set->used_len - 1);
  return ((set->used_len - 1) * sizeof (bitset_elt) * CHAR_BIT
          + bitset_elt_ffs (*elt)
          + 1);
}

/*
 * Local variables:
 * mode: C
 * indent-tabs-mode: nil
 * arch-tag: 8fab10b8-526e-4130-a295-cb6d120ad437
 * End:
 */
