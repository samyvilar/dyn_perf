
#ifndef __MEM_H__
#define __MEM_H__

#include <stddef.h>

#include "comp_utils.h"
#include "scalrs.h"
#include "vect.h"
#include "sse.h"


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

#define mem_inline_sign static_inline void * __attribute__((__always_inline__))


static_inline void *malloc_align(const size_t byte_cnt) {
    void *const temp = malloc(byte_cnt + _s(lrgst_vect_ingtl_t));
    return (void *)((uword_t)temp + (_s(lrgst_vect_ingtl_t) - ((uword_t)temp % _s(lrgst_vect_ingtl_t))));
}
static_inline void free_align(void *const temp) {
    free((void *)((uword_t )temp - (_s(lrgst_vect_ingtl_t) - ((uword_t)temp % _s(lrgst_vect_ingtl_t)))));
}

static_inline void *mem_clr_align(void *dest, const size_t byte_cnt) {
    typedef vect_lrgst_intgl_type oprn_t;

    const _t(vect.lrgst.intgl.ops->store_align) set = vect.lrgst.intgl.ops->store_align;
    const oprn_t zero = vect.lrgst.intgl.ops->setzeros();

    size_t curr;
    for (curr = 0; curr < (byte_cnt % _s(oprn_t)); curr++)
        ((unsigned char *)dest)[curr] = 0;

    switch ((byte_cnt / _s(zero)) % 2) {
        case 1: set((oprn_t *)&dest[curr], zero);
                curr += _s(zero);

        case 0: for (; curr < byte_cnt; curr += (2 * _s(zero))) {
                set((oprn_t *)&dest[curr],              zero);
                set((oprn_t *)&dest[curr + _s(oprn_t)], zero);
            }
    }
    return dest;
}
#define macro_unroll(func, times, args...)  \
    _t(times) block_cnt = (times) / 4;      \
    switch ((times) % 4) {                  \
        case 0: do {                        \
                func(args);                 \
        case 3: func(args);                 \
        case 2: func(args);                 \
        case 1: func(args);                 \
        } while (block_cnt--);              \
    }

#endif
