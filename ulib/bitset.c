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

static unsigned int
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

  assert (set->used_len > 0);
  if (set->used_len > 1)
    {
      i = set->used_len - 1;
      elt = ulib_vector_elt (&set->bits, i - 1);
      while (i > 0 && *elt == 0)
        {
          --i;
          --elt;
        }

      return i;
    }
  else
    return 0;
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

  elt = ulib_vector_front (&set->bits);
  if (set->used_len < idx + 1)
    {
      memset (elt + set->used_len, 0,
              (idx - set->used_len + 1) * sizeof (bitset_elt));
      set->used_len = idx + 1;
    }
  elt [idx] |= 1U << bit_index (n);

  return 0;
}

/* Set each bit, whose index is in the array IDX of length N.  */
int
ulib_bitset_set_bits (ulib_bitset *set, unsigned int n,
                      const unsigned int *idx)
{
  unsigned int i, mx, mx_idx;
  bitset_elt *elt;

  if (n == 0)
    return 0;

  /* Get the max index.  */
  mx = 0;
  for (i = 0; i < n; ++i)
    if (mx < idx [i])
      mx = idx [i];

  /* Ensure the bitset is big enough.  */
  mx_idx = elt_index (mx);
  if (ulib_vector_atleast (&set->bits, mx_idx + 1) < 0)
    return -1;

  /* Check if we need to increase used elements count and clear the
     new elements.  */
  elt = ulib_vector_front (&set->bits);
  if (set->used_len < mx_idx + 1)
    {
      memset (elt + set->used_len, 0,
              (mx_idx - set->used_len + 1) * sizeof (bitset_elt));
      set->used_len = mx_idx + 1;
    }

  /* Set bits.  */
  for (i = 0; i < n; ++i)
    elt [elt_index (idx [i])] |= 1U << bit_index (idx [i]);

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

      elt = ulib_vector_front (&set->bits);
      elt [idx] &= ~(1U << bit_index (n));

      if (elt [idx] == 0 && idx == set->used_len - 1)
        set->used_len = used_len (set);
    }
}

/* Clear each bit, whose index is in the array IDX of length N.  */
void
ulib_bitset_clear_bits (ulib_bitset *set, unsigned int n,
                        const unsigned int *idx)
{
  unsigned int i;
  bitset_elt *elt;

  if (set->used_len == 0)
    return;

  elt = ulib_vector_front (&set->bits);
  for (i = 0; i < n; ++i)
    {
      if (elt_index (idx [i]) < set->used_len)
        elt [elt_index (idx [i])] &= ~(1U << bit_index (idx [i]));
    }
  
  if (elt [set->used_len - 1] == 0)
    set->used_len = used_len (set);
}

/* Check whether bit N in the set SET is non-zero.  */
int
ulib_bitset_is_set (const ulib_bitset *set, unsigned int n)
{
  unsigned int idx = elt_index (n);

  if (idx >= set->used_len)
    return 0;
  else
    {
      const bitset_elt *elt;

      elt = ulib_vector_elt (&set->bits, idx);

      return *elt & (1U << bit_index (n));
    }
}


/* Set theoretic union: DST = SRC1 | SRC2.  */
int
ulib_bitset_or (ulib_bitset *restrict dst, const ulib_bitset *restrict src1,
                const ulib_bitset *restrict src2)
{
  unsigned int i, mn, mx;
  bitset_elt *restrict d;
  const bitset_elt *restrict s1, *restrict s2;

  mx = max_uint (src1->used_len, src2->used_len);
  
  if (ulib_vector_set_size (&dst->bits, mx) < 0)
    return -1;
  dst->used_len = mx;
  if (mx == 0)
    return 0;

  mn = min_uint (src1->used_len, src2->used_len);

  d = ulib_vector_front (&dst->bits);
  s1 = ulib_vector_front (&src1->bits);
  s2 = ulib_vector_front (&src2->bits);

  for (i = 0; i < mn; ++i)
    *d++ = *s1++ | *s2++;

  {
    const bitset_elt *restrict s3 = (i < src1->used_len) ? s1 : s2;

    for (; i < mx; ++i)
      *d++ = *s3++;
  }

  return 0;
}

/* Destructive set theoretic union: DST |= SRC.  */
int
ulib_bitset_destr_or (ulib_bitset *restrict dst,
                      const ulib_bitset *restrict src)
{
  unsigned int i, mn, mx;
  bitset_elt *restrict d;
  const bitset_elt *restrict s;

  if ((mx = max_uint (dst->used_len, src->used_len)) == 0)
    return 0;

  if (ulib_vector_set_size (&dst->bits, mx) < 0)
    return -1;

  mn = min_uint (dst->used_len, src->used_len);

  d = ulib_vector_front (&dst->bits);
  s = ulib_vector_front (&src->bits);

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

/* Destructive set theoretic union: DST |= SRC.  Returns negative on
   error, positive if the DST changed or zero otherwise.  */
int
ulib_bitset_destr_or_chg (ulib_bitset *restrict dst,
                          const ulib_bitset *restrict src)
{
  unsigned int i, mn, mx;
  bitset_elt *restrict d;
  const bitset_elt *restrict s;
  bitset_elt chg, tmp;

  if ((mx = max_uint (dst->used_len, src->used_len)) == 0)
    return 0;

  if (ulib_vector_set_size (&dst->bits, mx) < 0)
    return -1;

  mn = min_uint (dst->used_len, src->used_len);

  d = ulib_vector_front (&dst->bits);
  s = ulib_vector_front (&src->bits);
  for (i = 0, chg = 0; i < mn; ++i)
    {
      tmp = *d | *s++;
      chg |= *d ^ tmp;
      *d++ = tmp;
    }

  if (i < src->used_len)
    {
      chg = 1;
      for (; i < mx; ++i)
        *d++ = *s++;
    }
  dst->used_len = mx;

  return !!chg;
}


/* Set theoretic intersection: DST = SRC1 & SRC2.  */
int
ulib_bitset_and (ulib_bitset *restrict dst,
                 const ulib_bitset *restrict src1,
                 const ulib_bitset *restrict src2)
{
  unsigned int i, mn;
  bitset_elt *restrict d;
  const bitset_elt *restrict s1, *restrict s2;

  mn = min_uint (src1->used_len, src2->used_len);

  if (ulib_vector_set_size (&dst->bits, mn) < 0)
    return -1;
  dst->used_len = mn;
  if (mn == 0)
    return 0;

  d = ulib_vector_front (&dst->bits);
  s1 = ulib_vector_front (&src1->bits);
  s2 = ulib_vector_front (&src2->bits);

  for (i = 0; i < mn; ++i)
    *d++ = *s1++ & *s2++;

  if (d [-1] == 0)
    dst->used_len = used_len (dst);

  return 0;
}

/* Destructive set theoretic intersection: DST &= SRC.  */
int
ulib_bitset_destr_and (ulib_bitset *restrict dst,
                       const ulib_bitset *restrict src)
{
  unsigned int i, mn;
  bitset_elt *restrict d;
  const bitset_elt *restrict s;

  mn = min_uint (dst->used_len, src->used_len);

  if (ulib_vector_set_size (&dst->bits, mn) < 0)
    return -1;
  dst->used_len = mn;
  if (mn == 0)
    return 0;

  d = ulib_vector_front (&dst->bits);
  s = ulib_vector_front (&src->bits);

  for (i = 0; i < mn; ++i)
    *d++ &= *s++;

  if (d [-1] == 0)
    dst->used_len = used_len (dst);

  return 0;
}

/* Destructive set theoretic intersection: DST &= SRC.  Returns
   negative on error, positive if the DST changed or zero
   otherwise. */
int
ulib_bitset_destr_and_chg (ulib_bitset *restrict dst,
                           const ulib_bitset *restrict src)
{
  unsigned int i, mn;
  bitset_elt *restrict d;
  const bitset_elt *restrict s;
  bitset_elt chg, tmp;

  mn = min_uint (dst->used_len, src->used_len);

  if (ulib_vector_set_size (&dst->bits, mn) < 0)
    return -1;
  if (mn == 0)
    {
      if (dst->used_len == 0)
        return 0;
      else
        {
          dst->used_len = mn;
          return 1;
        }
    }

  d = ulib_vector_front (&dst->bits);
  s = ulib_vector_front (&src->bits);

  for (i = 0, chg = 0; i < mn; ++i)
    {
      tmp = *d & *s++;
      chg |= *d ^ tmp;
      *d++ = tmp;
    }

  if (mn < dst->used_len)
    {
      chg = 1;
      dst->used_len = mn;
    }

  if (d [-1] == 0)
    dst->used_len = used_len (dst);

  return !!chg;
}


/* Set theoretic difference: DST = SRC1 & ~SRC2.  */
int
ulib_bitset_andn (ulib_bitset *restrict dst,
                  const ulib_bitset *restrict src1,
                  const ulib_bitset *restrict src2)
{
  unsigned int i, mn;
  bitset_elt *restrict d;
  const bitset_elt *restrict s1, *restrict s2;

  if (ulib_vector_set_size (&dst->bits, src1->used_len))
    return -1;
  dst->used_len = src1->used_len;
  if (dst->used_len == 0)
    return 0;

  mn = min_uint (src1->used_len, src2->used_len);

  d = ulib_vector_front (&dst->bits);
  s1 = ulib_vector_front (&src1->bits);
  s2 = ulib_vector_front (&src2->bits);

  for (i = 0; i < mn; ++i)
    *d++ = *s1++ & ~ *s2++;

  for (; i < src1->used_len; ++i)
    *d++ = *s1++;

  if (d [-1] == 0)
    dst->used_len = used_len (dst);

  return 0;
}

/* Destructive set theoretic difference: DST &= ~SRC.  */
int
ulib_bitset_destr_andn (ulib_bitset *restrict dst,
                        const ulib_bitset *restrict src)
{
  unsigned int i, mn;
  bitset_elt *restrict d;
  const bitset_elt *restrict s;

  mn = min_uint (dst->used_len, src->used_len);
  if (mn == 0)
    return 0;

  d = ulib_vector_front (&dst->bits);
  s = ulib_vector_front (&src->bits);

  for (i = 0; i < mn; ++i)
    *d++ &= ~ *s++;

  d += dst->used_len - mn;

  if (d [-1] == 0)
    dst->used_len = used_len (dst);

  return 0;
}

/* Destructive set theoretic difference: DST &= ~SRC.  Returns
   negative on error, positive if the DST changed or zero
   otherwise.  */
int
ulib_bitset_destr_andn_chg (ulib_bitset *restrict dst,
                            const ulib_bitset *restrict src)
{
  unsigned int i, mn;
  bitset_elt *restrict d;
  const bitset_elt *restrict s;
  bitset_elt tmp, chg;

  mn = min_uint (dst->used_len, src->used_len);
  if (mn == 0)
    return 0;

  d = ulib_vector_front (&dst->bits);
  s = ulib_vector_front (&src->bits);

  for (i = 0, chg = 0; i < mn; ++i)
    {
      tmp = *d & ~ *s++;
      chg |= *d ^ tmp;
      *d++ = tmp;
    }

  d += dst->used_len - mn;

  if (d [-1] == 0)
    dst->used_len = used_len (dst);

  return !!chg;
}


/* Compute DST = SRC1 | (SRC2 & ~SRC3)  */
int ulib_bitset_or_andn (ulib_bitset *restrict dst,
                         const ulib_bitset *restrict src1,
                         const ulib_bitset *restrict src2,
                         const ulib_bitset *restrict src3)
{
  unsigned int i, mn, mx;
  bitset_elt *restrict d;
  const bitset_elt *restrict s1, *restrict s2, *restrict s3;

  mx = max_uint (src1->used_len, src2->used_len);
  
  if (ulib_vector_set_size (&dst->bits, mx) < 0)
    return -1;
  dst->used_len = mx;
  if (mx == 0)
    return 0;

  mn = min_uint (src1->used_len, src2->used_len);
  mn = min_uint (mn, src3->used_len);

  d = ulib_vector_front (&dst->bits);
  s1 = ulib_vector_front (&src1->bits);
  s2 = ulib_vector_front (&src2->bits);
  s3 = ulib_vector_front (&src3->bits);

  for (i = 0; i < mn; ++i)
    *d++ = *s1++ | (*s2++ & ~*s3++);

  if (i == src1->used_len)
    {
      mn = min_uint (src2->used_len, src3->used_len);
      for (; i < mn; ++i)
        *d++ = *s2++ & ~*s3++;

      if (i == src3->used_len)
        {
          for (; i < src2->used_len; ++i)
            *d++ = *s2++;
        }
    }
  else if (mn == src2->used_len)
    {
      for (; i < src1->used_len; ++i)
        *d++ = *s1++;
    }
  else
    {
      mn = min_uint (src1->used_len, src2->used_len);
      for (; i < mn; ++i)
        *d++ = *s1++ | *s2++;

      if (i == src1->used_len)
        {
          for (; i < src2->used_len; ++i)
            *d++ = *s2++;
        }
      else
        {
          for (; i < src1->used_len; ++i)
            *d++ = *s1++;
        }
    }

  return 0;
}

/* Compute DST |=  SRC1 & ~SRC2  */
int
ulib_bitset_destr_or_andn (ulib_bitset *restrict dst,
                           const ulib_bitset *restrict src1,
                           const ulib_bitset *restrict src2)
{
  unsigned int i, mn, mx;
  bitset_elt *restrict d;
  const bitset_elt *restrict s1, *restrict s2;
  bitset_elt tmp;

  if ((mx = max_uint (dst->used_len, src1->used_len)) == 0)
    return 0;

  if (ulib_vector_set_size (&dst->bits, mx) < 0)
    return -1;

  mn = min_uint (dst->used_len, src1->used_len);
  mn = min_uint (mn, src2->used_len);

  d = ulib_vector_front (&dst->bits);
  s1 = ulib_vector_front (&src1->bits);
  s2 = ulib_vector_front (&src2->bits);

  for (i = 0; i < mn; ++i)
    {
      tmp = *d | (*s1++ & ~*s2++);
      *d++ = tmp;
    }

  if (i == dst->used_len)
    {
      mn = min_uint (src1->used_len, src2->used_len);
      for (; i < mn; ++i)
        *d++ = *s1++ & ~*s2++;

      if (i == src2->used_len)
        {
          for (; i < src1->used_len; ++i)
            *d++ = *s1++;
        }
    }
  else if (mn == src2->used_len)
    {
      mn = min_uint (dst->used_len, src1->used_len);
      for (; i < mn; ++i)
        {
          tmp = *d | *s1++;
          *d++ = tmp;
        }

      if (i == dst->used_len)
        {
          for (; i < src1->used_len; ++i)
            *d++ = *s1++;
        }
    }

  dst->used_len = mx;

  return 0;
}

/* Compute DST = (DST | SRC1) & ~SRC2.  Returns negative on error,
   positive if the DST changed or zero otherwise.  */
int
ulib_bitset_destr_or_andn_chg (ulib_bitset *restrict dst,
                               const ulib_bitset *restrict src1,
                               const ulib_bitset *restrict src2)
{
  unsigned int i, mn, mx;
  bitset_elt *restrict d;
  const bitset_elt *restrict s1, *restrict s2;
  bitset_elt chg, tmp;

  if ((mx = max_uint (dst->used_len, src1->used_len)) == 0)
    return 0;

  if (ulib_vector_set_size (&dst->bits, mx) < 0)
    return -1;

  mn = min_uint (dst->used_len, src1->used_len);
  mn = min_uint (mn, src2->used_len);

  d = ulib_vector_front (&dst->bits);
  s1 = ulib_vector_front (&src1->bits);
  s2 = ulib_vector_front (&src2->bits);

  chg = 0;
  for (i = 0; i < mn; ++i)
    {
      tmp = *d | (*s1++ & ~*s2++);
      chg |= *d ^ tmp;
      *d++ = tmp;
    }

  if (i == dst->used_len)
    {
      mn = min_uint (src1->used_len, src2->used_len);
      for (; i < mn; ++i)
        {
          tmp = *s1++ & ~*s2++;
          chg |= *d ^ tmp;
          *d++ = tmp;
        }

      if (i == src2->used_len)
        {
          for (; i < src1->used_len; ++i)
            {
              chg |= *d ^ *s1;
              *d++ = *s1++;
            }
        }
    }
  else if (mn == src2->used_len)
    {
      mn = min_uint (dst->used_len, src1->used_len);
      for (; i < mn; ++i)
        {
          tmp = *d | *s1++;
          chg |= *d ^ tmp;
          *d++ = tmp;
        }

      if (i == dst->used_len)
        {
          for (; i < src1->used_len; ++i)
            {
              chg |= *d ^ *s1;
              *d++ = *s1++;
            }
        }
    }

  dst->used_len = mx;

  return !!chg;
}


/* Return one plus the index of the last bit set.  */
unsigned int
ulib_bitset_max (const ulib_bitset *set)
{
  const bitset_elt *elt;

  if (set->used_len == 0)
    return 0;

  elt = ulib_vector_elt (&set->bits, set->used_len - 1);
  return ((set->used_len - 1) * sizeof (bitset_elt) * CHAR_BIT
          + bitset_elt_ffs (*elt)
          + 1);
}

/* Check whether set A is a subset of set B.  */
int
ulib_bitset_is_subset (const ulib_bitset *restrict a,
                       const ulib_bitset *restrict b)
{
  unsigned int n;
  const bitset_elt *elt_a, *elt_b;

  if (a->used_len == 0 || a->used_len < b->used_len)
    return 1;

  if (b->used_len == 0 || b->used_len < a->used_len)
    return 0;

  n = a->used_len;
  elt_a = ulib_vector_front (&a->bits);
  elt_b = ulib_vector_front (&b->bits);
  while (n--)
    {
      if (*elt_a != *elt_b && (*elt_b & ~ *elt_a) == 0)
        return 0;

      ++elt_a;
      ++elt_b;
    }

  return 1;
}

/*
 * Local variables:
 * mode: C
 * indent-tabs-mode: nil
 * End:
 */
