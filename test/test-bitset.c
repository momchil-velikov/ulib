#include <ulib/bitset.h>
#include <ulib/rand.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <assert.h>
#include <ulib/time.h>

#define BITS_MAX 1024U
#define BITS_PER_SET 500
#define NSETS_MAX 100

#ifdef NDEBUG
#define NLOOPS 10000000
#else
#define NLOOPS 100000
#endif

static ulib_bitset *set[NSETS_MAX];

/* Generate a random set of NBITS elements.  */
static void
random_set(unsigned int nbits, ulib_bitset *set) {
    unsigned int i, n;

    for (i = 0; i < nbits; ++i) {
        n = ulib_rand(0, BITS_MAX - 1);
        ulib_bitset_set(set, n);
    }
}

/* Allocate and populate a random set.  */
static ulib_bitset *
make_random_set(unsigned int nbits) {
    ulib_bitset *set;

    set = malloc(sizeof(ulib_bitset));
    ulib_bitset_init(set);
    random_set(nbits, set);

    return set;
}

/* Check whether DST is the union of SRC1 and SRC2.  */
static int
check_union(const ulib_bitset *dst, const ulib_bitset *src1, const ulib_bitset *src2) {
    unsigned int i, n, m;

    n = ulib_bitset_max(src1);
    m = ulib_bitset_max(src2);

    n = n > m ? n : m;

    /* Check whether each bit in SRC1 or SRC2 in present in DST.  */
    for (i = 0; i < n; ++i)
        if ((ulib_bitset_is_set(src1, i) != 0 || ulib_bitset_is_set(src2, i) != 0)
            && ulib_bitset_is_set(dst, i) == 0)
            return -1;

    /* Check whether each bit in DST is present in SRC1 or SRC2.  */
    n = ulib_bitset_max(dst);
    for (i = 0; i < n; ++i)
        if (ulib_bitset_is_set(dst, i) != 0 && ulib_bitset_is_set(src1, i) == 0
            && ulib_bitset_is_set(src2, i) == 0)
            return -1;

    return 0;
}

/* Check whether DST is the intersection of SRC1 and SRC2.  */
static int
check_intersection(const ulib_bitset *dst,
                   const ulib_bitset *src1,
                   const ulib_bitset *src2) {
    unsigned int i, n, m;

    n = ulib_bitset_max(src1);
    m = ulib_bitset_max(src2);

    n = n > m ? n : m;

    /* Check whether a bit, set in both SRC1 and SRC2, is set in DST
     too.  */
    for (i = 0; i < n; ++i)
        if (ulib_bitset_is_set(src1, i) != 0 && ulib_bitset_is_set(src2, i) != 0
            && ulib_bitset_is_set(dst, i) == 0)
            return -1;

    /* Check whether a bit, set in DST, is set in SRC1 and SRC2 too.  */
    n = ulib_bitset_max(dst);
    for (i = 0; i < n; ++i)
        if (ulib_bitset_is_set(dst, i) != 0
            && (ulib_bitset_is_set(src1, i) == 0 || ulib_bitset_is_set(src2, i) == 0))
            return -1;

    return 0;
}

/* Check whether DST is the set difference between SRC1 and SRC2.  */
static int
check_difference(const ulib_bitset *dst,
                 const ulib_bitset *src1,
                 const ulib_bitset *src2) {
    unsigned int i, n, m;

    n = ulib_bitset_max(src1);
    m = ulib_bitset_max(src2);

    n = n > m ? n : m;

    /* Check whether each bit in SRC2 is not present in DST.  */
    for (i = 0; i < n; ++i)
        if (ulib_bitset_is_set(src2, i) != 0 && ulib_bitset_is_set(dst, i) != 0)
            return -1;

    /* Check whether each bit in DST is set in SRC1 and clear in
     SRC2.  */
    n = ulib_bitset_max(dst);
    for (i = 0; i < n; ++i)
        if (ulib_bitset_is_set(dst, i) != 0
            && (ulib_bitset_is_set(src1, i) == 0 || ulib_bitset_is_set(src2, i) != 0))
            return -1;

    return 0;
}

/* Check whether all bits with indices in IDX are 1 in SET.  */
static int
check_bits_set(const ulib_bitset *set, unsigned int n, const unsigned int *idx) {
    while (n--)
        if (!ulib_bitset_is_set(set, idx[n]))
            return -1;

    return 0;
}

/* Check whether all bits with indices in IDX are 0 in SET.  */
static int
check_bits_clear(const ulib_bitset *set, unsigned int n, const unsigned int *idx) {
    while (n--)
        if (ulib_bitset_is_set(set, idx[n]))
            return -1;

    return 0;
}

/* Allocate and populate the test sets.  */
static void
init_sets() {
    unsigned int i;

    for (i = 0; i < NSETS_MAX; ++i)
        set[i] = make_random_set(BITS_PER_SET);
}

/* Destroy the test sets.  */
static void
destroy_sets() {
    unsigned int i;

    for (i = 0; i < NSETS_MAX; ++i) {
        ulib_bitset_destroy(set[i]);
        free(set[i]);
    }
}

/* Test setting many bits: one by one.  */
static void
test_bitset_set_one_by_one() {
    unsigned int i, j, sts;
    unsigned int *idx;
    ulib_bitset set;

    for (i = 0; i < NLOOPS / 100; ++i) {
        sts = ulib_bitset_init(&set);
        assert(sts == 0);

        idx = malloc(BITS_PER_SET * sizeof(unsigned int));
        assert(idx);

        for (j = 0; j < BITS_PER_SET; ++j) {
            idx[j] = ulib_rand(0, BITS_MAX - 1);
            sts = ulib_bitset_set(&set, idx[j]);
            assert(sts == 0);
        }

        assert(check_bits_set(&set, BITS_PER_SET, idx) == 0);
        ulib_bitset_destroy(&set);
        free(idx);
    }
}

/* Test setting many bits: at once.  */
static void
test_bitset_set_at_once() {
    unsigned int i, j, sts;
    unsigned int *idx;
    ulib_bitset set;

    for (i = 0; i < NLOOPS / 100; ++i) {
        sts = ulib_bitset_init(&set);
        assert(sts == 0);

        idx = malloc(BITS_PER_SET * sizeof(unsigned int));
        assert(idx);

        for (j = 0; j < BITS_PER_SET; ++j)
            idx[j] = ulib_rand(0, BITS_MAX - 1);

        sts = ulib_bitset_set_bits(&set, BITS_PER_SET, idx);
        assert(sts == 0);

        assert(check_bits_set(&set, BITS_PER_SET, idx) == 0);
        ulib_bitset_destroy(&set);
        free(idx);
    }
}

/* Test clearing many bits: one by one.  */
static void
test_bitset_clear_one_by_one() {
    unsigned int i, j, d;
    unsigned int *idx;

    for (i = 0; i < NLOOPS / 100; ++i) {
        d = ulib_rand(0, NSETS_MAX - 1);

        idx = malloc(BITS_PER_SET * sizeof(unsigned int));
        assert(idx);

        for (j = 0; j < BITS_PER_SET; ++j) {
            idx[j] = ulib_rand(0, BITS_MAX - 1);
            ulib_bitset_clear(set[d], idx[j]);
        }

        assert(check_bits_clear(set[d], BITS_PER_SET, idx) == 0);
        free(idx);
    }
}

/* Test clearing many bits: at once.  */
static void
test_bitset_clear_at_once() {
    unsigned int i, j, d;
    unsigned int *idx;

    for (i = 0; i < NLOOPS / 100; ++i) {
        d = ulib_rand(0, NSETS_MAX - 1);

        idx = malloc(BITS_PER_SET * sizeof(unsigned int));
        assert(idx);

        for (j = 0; j < BITS_PER_SET; ++j)
            idx[j] = ulib_rand(0, BITS_MAX - 1);

        ulib_bitset_clear_bits(set[d], BITS_PER_SET, idx);

        assert(check_bits_clear(set[d], BITS_PER_SET, idx) == 0);
        free(idx);
    }
}

/* Test union operation.  */
static void
test_bitset_or() {
    volatile int sts;
    unsigned int i, d, s1, s2;

    for (i = 0; i < NLOOPS; ++i) {
        do {
            d = ulib_rand(0, NSETS_MAX - 1);
            s1 = ulib_rand(0, NSETS_MAX - 1);
            s2 = ulib_rand(0, NSETS_MAX - 1);
        } while (d == s1 || d == s2 || s1 == s2);

        sts = ulib_bitset_or(set[d], set[s1], set[s2]);
        assert(sts == 0);

        assert(check_union(set[d], set[s1], set[s2]) == 0);
    }
}

static void
test_bitset_destr_or() {
    volatile int sts;
    unsigned int i, d, s;

    for (i = 0; i < NLOOPS; ++i) {
        do {
            d = ulib_rand(0, NSETS_MAX - 1);
            s = ulib_rand(0, NSETS_MAX - 1);
        } while (d == s);

        sts = ulib_bitset_destr_or(set[d], set[s]);
        assert(sts == 0);

        assert(check_union(set[d], set[d], set[s]) == 0);
    }
}

static void
test_bitset_destr_or_chg() {
    volatile int sts;
    unsigned int i, d, s1, s2, b;

    for (i = 0; i < NLOOPS; ++i) {
        do {
            d = ulib_rand(0, NSETS_MAX - 1);
            s1 = ulib_rand(0, NSETS_MAX - 1);
            s2 = ulib_rand(0, NSETS_MAX - 1);
        } while (d == s1 || d == s2 || s1 == s2);

        b = ulib_rand(0, BITS_MAX - 1);
        ulib_bitset_clear(set[s1], b);
        ulib_bitset_clear(set[s2], b);

        sts = ulib_bitset_or(set[d], set[s1], set[s2]);
        assert(sts == 0);

        sts = ulib_bitset_destr_or_chg(set[d], set[s1]);
        assert(sts == 0);

        sts = ulib_bitset_destr_or_chg(set[d], set[s2]);
        assert(sts == 0);

        sts = ulib_bitset_set(set[s1], b);
        assert(sts == 0);

        sts = ulib_bitset_destr_or_chg(set[d], set[s1]);
        assert(sts > 0);
    }
}

/* Test intersection operation.  */
static void
test_bitset_and() {
    volatile int sts;
    unsigned int i, d, s1, s2;

    for (i = 0; i < NLOOPS; ++i) {
        do {
            d = ulib_rand(0, NSETS_MAX - 1);
            s1 = ulib_rand(0, NSETS_MAX - 1);
            s2 = ulib_rand(0, NSETS_MAX - 1);
        } while (d == s1 || d == s2 || s1 == s2);

        sts = ulib_bitset_and(set[d], set[s1], set[s2]);
        assert(sts == 0);

        assert(check_intersection(set[d], set[s1], set[s2]) == 0);
    }
}

static void
test_bitset_destr_and() {
    volatile int sts;
    unsigned int i, d, s;

    for (i = 0; i < NLOOPS; ++i) {
        do {
            d = ulib_rand(0, NSETS_MAX - 1);
            s = ulib_rand(0, NSETS_MAX - 1);
        } while (d == s);

        sts = ulib_bitset_destr_and(set[d], set[s]);
        assert(sts == 0);

        assert(check_intersection(set[d], set[d], set[s]) == 0);
    }
}

static void
test_bitset_destr_and_chg() {
    volatile int sts;
    unsigned int i, d, s1, s2, b;

    for (i = 0; i < NLOOPS; ++i) {
        do {
            d = ulib_rand(0, NSETS_MAX - 1);
            s1 = ulib_rand(0, NSETS_MAX - 1);
            s2 = ulib_rand(0, NSETS_MAX - 1);
        } while (d == s1 || d == s2 || s1 == s2);

        b = ulib_rand(0, BITS_MAX - 1);

        ulib_bitset_set(set[s1], b);
        ulib_bitset_set(set[s2], b);

        sts = ulib_bitset_and(set[d], set[s1], set[s2]);
        assert(sts == 0);

        sts = ulib_bitset_destr_and_chg(set[d], set[s1]);
        assert(sts == 0);

        sts = ulib_bitset_destr_and_chg(set[d], set[s2]);
        assert(sts == 0);

        ulib_bitset_clear(set[s1], b);

        sts = ulib_bitset_destr_and_chg(set[d], set[s1]);
        assert(sts > 0);
    }
}

/* Test difference operation.  */
static void
test_bitset_andn() {
    volatile int sts;
    unsigned int i, d, s1, s2;

    for (i = 0; i < NLOOPS; ++i) {
        do {
            d = ulib_rand(0, NSETS_MAX - 1);
            s1 = ulib_rand(0, NSETS_MAX - 1);
            s2 = ulib_rand(0, NSETS_MAX - 1);
        } while (d == s1 || d == s2 || s1 == s2);

        sts = ulib_bitset_andn(set[d], set[s1], set[s2]);
        assert(sts == 0);

        assert(check_difference(set[d], set[s1], set[s2]) == 0);
    }
}

static void
test_bitset_destr_andn() {
    volatile int sts;
    unsigned int i, d, s;

    for (i = 0; i < NLOOPS; ++i) {
        do {
            d = ulib_rand(0, NSETS_MAX - 1);
            s = ulib_rand(0, NSETS_MAX - 1);
        } while (d == s);

        sts = ulib_bitset_destr_andn(set[d], set[s]);
        assert(sts == 0);

        assert(check_difference(set[d], set[d], set[s]) == 0);
    }
}

static void
test_bitset_destr_andn_chg() {
    volatile int sts;
    unsigned int i, d, s1, s2, b;

    for (i = 0; i < NLOOPS; ++i) {
        do {
            d = ulib_rand(0, NSETS_MAX - 1);
            s1 = ulib_rand(0, NSETS_MAX - 1);
            s2 = ulib_rand(0, NSETS_MAX - 1);
        } while (d == s1 || d == s2 || s1 == s2);

        b = ulib_rand(0, BITS_MAX - 1);
        sts = ulib_bitset_set(set[s1], b);
        assert(sts == 0);
        ulib_bitset_clear(set[s2], b);

        sts = ulib_bitset_andn(set[d], set[s1], set[s2]);
        assert(sts == 0);

        sts = ulib_bitset_destr_andn_chg(set[d], set[s2]);
        assert(sts == 0);

        sts = ulib_bitset_set(set[s2], b);
        assert(sts == 0);

        sts = ulib_bitset_destr_andn_chg(set[d], set[s2]);
        assert(sts > 0);
    }
}

static void
test_bitset_or_andn() {
    volatile int sts;
    unsigned int i, d, s1, s2, s3;

    for (i = 0; i < NLOOPS; ++i) {
        do {
            d = ulib_rand(0, NSETS_MAX - 1);
            s1 = ulib_rand(0, NSETS_MAX - 1);
            s2 = ulib_rand(0, NSETS_MAX - 1);
            s3 = ulib_rand(0, NSETS_MAX - 1);
        } while (d == s1 || d == s2 || d == s3 || s1 == s2 || s1 == s3 || s2 == s3);

        sts = ulib_bitset_or_andn(set[d], set[s1], set[s2], set[s3]);
        assert(sts == 0);

#ifndef NDEBUG
        {
            int sts;
            unsigned int n = ulib_bitset_max(set[d]);

            while (n--) {
                sts = (ulib_bitset_is_set(set[s1], n)
                       || (ulib_bitset_is_set(set[s2], n)
                           && !ulib_bitset_is_set(set[s3], n)));
                assert((sts == 0 && !ulib_bitset_is_set(set[d], n))
                       || (sts != 0 && ulib_bitset_is_set(set[d], n)));
            }
        }
#endif
    }
}

static void
test_bitset_destr_or_andn() {
    volatile int sts;
    unsigned int i, d, s1, s2;
    ulib_bitset *d0 = set[0];

    for (i = 0; i < NLOOPS; ++i) {
        do {
            d = ulib_rand(1, NSETS_MAX - 1);
            s1 = ulib_rand(1, NSETS_MAX - 1);
            s2 = ulib_rand(1, NSETS_MAX - 1);
        } while (d == s1 || d == s2 || s1 == s2);

        sts = ulib_bitset_copy(d0, set[d]);
        assert(sts == 0);

        sts = ulib_bitset_destr_or_andn(set[d], set[s1], set[s2]);
        assert(sts == 0);

#ifndef NDEBUG
        {
            int sts;
            unsigned int n = ulib_bitset_max(set[d]);

            while (n--) {
                sts = (ulib_bitset_is_set(d0, n) || (ulib_bitset_is_set(set[s1], n)
                                                     && !ulib_bitset_is_set(set[s2], n)));
                assert((sts == 0 && !ulib_bitset_is_set(set[d], n))
                       || (sts != 0 && ulib_bitset_is_set(set[d], n)));
            }
        }
#endif
    }
}

static void
test_bitset_destr_or_andn_chg() {
    volatile int sts;
    unsigned int i, d, s1, s2, b;

    for (i = 0; i < NLOOPS; ++i) {
        do {
            d = ulib_rand(0, NSETS_MAX - 1);
            s1 = ulib_rand(0, NSETS_MAX - 1);
            s2 = ulib_rand(0, NSETS_MAX - 1);
        } while (d == s1 || d == s2 || s1 == s2);

        b = ulib_rand(0, BITS_MAX);
        ulib_bitset_clear(set[d], b);
        ulib_bitset_clear(set[s1], b);
        ulib_bitset_clear(set[s2], b);

        sts = ulib_bitset_destr_or_andn(set[d], set[s1], set[s2]);
        assert(sts == 0);

        sts = ulib_bitset_destr_or_andn_chg(set[d], set[s1], set[s2]);
        assert(sts == 0);

        ulib_bitset_set(set[s1], b);
        sts = ulib_bitset_destr_or_andn_chg(set[d], set[s1], set[s2]);
        assert(sts > 0);

        ulib_bitset_set(set[s2], b);
        sts = ulib_bitset_destr_or_andn_chg(set[d], set[s1], set[s2]);
        assert(sts == 0);

        ulib_bitset_clear(set[d], b);
        sts = ulib_bitset_destr_or_andn_chg(set[d], set[s1], set[s2]);
        assert(sts == 0);
    }
}

/* Test subset operation.  */
static void
test_bitset_subset() {
    volatile int sts;
    unsigned int i, s1, s2;
    ulib_bitset *dst = set[0];

    for (i = 0; i < NLOOPS; ++i) {
        do {
            s1 = ulib_rand(1, NSETS_MAX - 1);
            s2 = ulib_rand(1, NSETS_MAX - 1);
        } while (s1 == s2);

        sts = ulib_bitset_or(dst, set[s1], set[s2]);
        assert(sts == 0);

        sts = ulib_bitset_is_subset(set[s1], dst);
        assert(sts != 0);

        sts = ulib_bitset_is_subset(set[s2], dst);
        assert(sts != 0);

        sts = ulib_bitset_and(dst, set[s1], set[s2]);
        assert(sts == 0);

        sts = ulib_bitset_is_subset(dst, set[s1]);
        assert(sts != 0);

        sts = ulib_bitset_is_subset(dst, set[s2]);
        assert(sts != 0);
    }
}

/* Compute timestamp difference. Result in microseconds.  */
static double
diffts(const ulib_time *begin, const ulib_time *end) {
    return (end->sec * 1e6 + end->usec - begin->sec * 1e6 - begin->usec);
}

int
main() {
    ulib_time ts1, ts2;

    ulib_gettime(&ts1);
    test_bitset_set_one_by_one();
    ulib_gettime(&ts2);
    printf("set[one by one]: %f s\n", diffts(&ts1, &ts2) / 1e6);

    ulib_gettime(&ts1);
    test_bitset_set_at_once();
    ulib_gettime(&ts2);
    printf("set[at once]: %f s\n", diffts(&ts1, &ts2) / 1e6);

    init_sets();
    ulib_gettime(&ts1);
    test_bitset_clear_one_by_one();
    ulib_gettime(&ts2);
    destroy_sets();
    printf("clear[one by one]: %f s\n", diffts(&ts1, &ts2) / 1e6);

    init_sets();
    ulib_gettime(&ts1);
    test_bitset_clear_at_once();
    ulib_gettime(&ts2);
    destroy_sets();
    printf("clear[at once]: %f s\n", diffts(&ts1, &ts2) / 1e6);

    init_sets();
    ulib_gettime(&ts1);
    test_bitset_or();
    ulib_gettime(&ts2);
    destroy_sets();
    printf("or: %f s\n", diffts(&ts1, &ts2) / 1e6);

    init_sets();
    ulib_gettime(&ts1);
    test_bitset_destr_or();
    ulib_gettime(&ts2);
    destroy_sets();
    printf("destr_or: %f s\n", diffts(&ts1, &ts2) / 1e6);

    init_sets();
    ulib_gettime(&ts1);
    test_bitset_destr_or_chg();
    ulib_gettime(&ts2);
    destroy_sets();
    printf("destr_or_chg: %f s\n", diffts(&ts1, &ts2) / 1e6);

    init_sets();
    ulib_gettime(&ts1);
    test_bitset_and();
    ulib_gettime(&ts2);
    destroy_sets();
    printf("and: %f s\n", diffts(&ts1, &ts2) / 1e6);

    init_sets();
    ulib_gettime(&ts1);
    test_bitset_destr_and();
    ulib_gettime(&ts2);
    destroy_sets();
    printf("destr_and: %f s\n", diffts(&ts1, &ts2) / 1e6);

    init_sets();
    ulib_gettime(&ts1);
    test_bitset_destr_and_chg();
    ulib_gettime(&ts2);
    destroy_sets();
    printf("destr_and_chg: %f s\n", diffts(&ts1, &ts2) / 1e6);

    init_sets();
    ulib_gettime(&ts1);
    test_bitset_andn();
    ulib_gettime(&ts2);
    destroy_sets();
    printf("andn: %f s\n", diffts(&ts1, &ts2) / 1e6);

    init_sets();
    ulib_gettime(&ts1);
    test_bitset_destr_andn();
    ulib_gettime(&ts2);
    destroy_sets();
    printf("destr_andn: %f s\n", diffts(&ts1, &ts2) / 1e6);

    init_sets();
    ulib_gettime(&ts1);
    test_bitset_destr_andn_chg();
    ulib_gettime(&ts2);
    destroy_sets();
    printf("destr_andn_chg: %f s\n", diffts(&ts1, &ts2) / 1e6);

    init_sets();
    ulib_gettime(&ts1);
    test_bitset_or_andn();
    ulib_gettime(&ts2);
    destroy_sets();
    printf("or_andn: %f s\n", diffts(&ts1, &ts2) / 1e6);

    init_sets();
    ulib_gettime(&ts1);
    test_bitset_destr_or_andn();
    ulib_gettime(&ts2);
    destroy_sets();
    printf("destr_or_andn: %f s\n", diffts(&ts1, &ts2) / 1e6);

    init_sets();
    ulib_gettime(&ts1);
    test_bitset_destr_or_andn_chg();
    ulib_gettime(&ts2);
    destroy_sets();
    printf("destr_or_andn_chg: %f s\n", diffts(&ts1, &ts2) / 1e6);

    init_sets();
    ulib_gettime(&ts1);
    test_bitset_subset();
    ulib_gettime(&ts2);
    destroy_sets();
    printf("subset: %f s\n", diffts(&ts1, &ts2) / 1e6);

    return 0;
}

/*
 * Local variables:
 * mode: C
 * indent-tabs-mode: nil
 * End:
 */
