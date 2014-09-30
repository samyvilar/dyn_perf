
#ifndef __MEM_H__
#define __MEM_H__

#include <stddef.h>
#include <xmmintrin.h>// <x86intrin.h>
#include <pmmintrin.h>
#include <emmintrin.h>


#include "comp_utils.h"
#include "scalrs.h"

#define array_cnt(arr) (sizeof(arr)/sizeof((arr)[0]))

typedef __m128i lrgst_vect_ingtl_t;
typedef void (*vect_ingl_storr)(lrgst_vect_ingtl_t *, lrgst_vect_ingtl_t);

#define lrgst_vect_storr            _mm_storeu_si128
#define lrgst_vect_storr_align      _mm_store_si128

typedef _t(&lrgst_vect_storr)       lrgst_vect_storr_t;

#define lrgst_vect_ingtl_setzero    _mm_setzero_si128

#define lrgst_vect_brdcst_intgl_8  &_mm_set1_epi8
#define lrgst_vect_brdcst_intgl_16 &_mm_set1_epi16
#define lrgst_vect_brdcst_intgl_32 &_mm_set1_epi32
#define lrgst_vect_brdcst_intgl_64 &_mm_set1_epi64x

#define lrgst_vect_brdcst_intgl(expr) \
scalr_switch_oblvs_sign_intgl(  \
    expr,                       \
    lrgst_vect_brdcst_intgl_64, \
    lrgst_vect_brdcst_intgl_32, \
    lrgst_vect_brdcst_intgl_16, \
    lrgst_vect_brdcst_intgl_8,  \
    (void)0                     \
)(expr)

#define mem_inline_sign static __inline__ void * __attribute__((__always_inline__))

mem_inline_sign mem_clr(void *const _src, unsigned long byte_cnt) {
    typedef lrgst_vect_ingtl_t oprn_t;
    typedef lrgst_vect_storr_t storr_t;

    unsigned char *const src = _src;

    register typeof(byte_cnt) consmd;
    for (consmd = 0; consmd < (byte_cnt % sizeof(oprn_t)); consmd++)
        src[consmd] = 0; // need byte_cnt to be a multiple of vect size ...

    if (comp_likely(consmd < byte_cnt)) { // <<<< remainder and cnt is a multiple of sizeof(oprn_t)
        register oprn_t *dest = (void *)&src[consmd], zero = lrgst_vect_ingtl_setzero();

        const storr_t store = ((uword_t)dest % sizeof(*dest)) ? lrgst_vect_storr : lrgst_vect_storr_align;

        byte_cnt -= consmd;
        consmd   = (byte_cnt / sizeof(*dest));
        byte_cnt = consmd % 4; // <<< get remainder
        consmd  /= 4; // <<<< get unrolled count

        switch (byte_cnt) { // clear 64 bytes at a time ...
            case 0: do {
                    store(dest++, zero);
            case 3: store(dest++, zero);
            case 2: store(dest++, zero);
            case 1: store(dest++, zero);
            } while (consmd--);
        }
    }
    return _src;
}

#define mem_set_pattrn(_dest, _pattern, times) ({                                    \
    typedef lrgst_vect_ingtl_t oprn_t;                                               \
    typedef lrgst_vect_storr_t storr_t;                                              \
                                                                                     \
    register _t(_pattern) *const items = (_dest), pattern = (_pattern);              \
    register size_t index;                                                           \
    for (index = 0; index < ((times) % (_s(oprn_t)/_s(pattern))); index++)           \
        items[index] = pattern;                                                      \
    register const oprn_t vect_pattrn = lrgst_vect_brdcst_intgl(pattern);            \
    register oprn_t *vect_dest = (oprn_t *)&items[index];                           \
    register word_t block_cnt  = ((times) / (_s(oprn_t)/_s(pattern))) / 4;           \
    const storr_t store = ((uword_t)vect_dest % _s(oprn_t))                         \
                    ? lrgst_vect_storr : lrgst_vect_storr_align;                     \
    switch (((times) / (_s(oprn_t)/_s(pattern))) % 4) {                              \
        case 0: do {                                                                 \
                store(vect_dest++, vect_pattrn);                                    \
        case 3: store(vect_dest++, vect_pattrn);                                    \
        case 2: store(vect_dest++, vect_pattrn);                                    \
        case 1: store(vect_dest++, vect_pattrn);                                    \
        } while (--block_cnt > 0);                                                      \
    }                                                                               \
    (void *)items;                                                                  \
})
 // TODO: it seems this won't compile under icc generating unknown symbol _mm_storeu_si128, _mm_store_si128

#endif
