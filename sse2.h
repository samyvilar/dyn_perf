
#ifndef __SSE_H__
#define __SSE_H__

#include <immintrin.h>
#include <stdio.h>

//#include <smmintrin.h>

#include "comp_utils.h"

typedef __m128i (*const sse_intgl_load_t)(const __m128i *);
typedef void (*const sse_intgl_store_t)(__m128i *, __m128i);

typedef __m128i (*const sse_bin_intgl_t)(__m128i, __m128i);

typedef __m128i (*const sse_bin_intgl_imm_int_t)(__m128i, int);

typedef __m128i (*const sse_unr_intgl_t)(__m128i);


static_inline __m128i intrsc_attrs _mm_is_neg_epi32(__m128i a) {
    return _mm_srai_epi32(a, 32);
}

static_inline __m128i intrsc_attrs _mm_is_neg_epi64(__m128i a) {
    return _mm_srai_epi32(_mm_shuffle_epi32(a, _MM_SHUFFLE(3, 3, 1, 1)), 32);
}

static_inline void sse_error_not_implemented(__m128i _, ...) {
    printf("ERROR: SSE operation not implemented!");
    exit(-1);
}

#if !defined(__SSE4_1__) && !defined(__INTEL_COMPILER)

    static_inline int _mm_extrt_epi8(__m128i a, const int imm) {
        if (!imm)
            return (char)(_mm_cvtsi128_si32(a) & 0xFF);

        if (imm & 1)
            return _mm_extract_epi16(a, imm/2) >> 8;

        return (char)(_mm_extract_epi16(a, imm/2) & 0xFF);
    }

    static_inline int intrsc_attrs _mm_extrt_epi32(__m128i a, const int imm) {
        switch (imm & 0b11) {
            case 0: return _mm_cvtsi128_si32(a);
            case 1: return _mm_cvtsi128_si32(_mm_srli_si128(a, 4));
            case 2: return _mm_cvtsi128_si32(_mm_srli_si128(a, 8));
            case 3: return _mm_cvtsi128_si32(_mm_srli_si128(a, 12));
        }
        __builtin_unreachable();
    }

    static_inline long long intrsc_attrs _mm_extrt_epi64(__m128i a, const int imm) {
        return _mm_cvtsi128_si64((imm & 1) ? _mm_srli_si128(a, 8) : a);
    }

    static_inline __m128i intrsc_attrs _mm_mullo_epi32(__m128i a, __m128i b) { // ~10 cycles
        return _mm_unpacklo_epi32(
            _mm_shuffle_epi32(_mm_mul_epu32(a, b), _MM_SHUFFLE(0,0,2,0)),
            _mm_shuffle_epi32(_mm_mul_epu32(_mm_srli_si128(a, 4), _mm_srli_si128(b, 4)), _MM_SHUFFLE(0,0,2,0))
        );
    }

    static_inline int intrsc_attrs _mm_test_all_ones(__m128i a) {
        return _mm_movemask_epi8(_mm_cmpeq_epi32(a, _mm_cmpeq_epi32(a, a))) == 0xFFFF;
    }

    static_inline int intrsc_attrs _mm_test_all_zeros(__m128i a, __m128i mask) {
        return _mm_movemask_epi8(_mm_cmpeq_epi32(_mm_and_si128(a, mask), _mm_andnot_si128(a, a))) == 0xFFFF;
    }

#else
#ifdef __INTEL_COMPILER
#   define _mm_extrt_epi32 _mm_extract_epi32
#   define _mm_extrt_epi64 _mm_extract_epi64
#else
    static_inline int       intrsc_attrs _mm_extrt_epi8(__m128i a, const int imm)  {return _mm_extract_epi8(a, imm); }
    static_inline int       intrsc_attrs _mm_extrt_epi32(__m128i a, const int imm) {return _mm_extract_epi32(a, imm);}
    static_inline long long intrsc_attrs _mm_extrt_epi64(__m128i a, const int imm) {return _mm_extract_epi64(a, imm);}
#endif
#endif

static_inline __m128i intrsc_attrs _mm_srli_epi8(__m128i a, const int b) {
    return _mm_and_si128(_mm_srli_epi16(a, b), _mm_set1_epi8(0xFFU >> (b & 0b111))); // 2-3 cycles ...
}

static_inline __m128i intrsc_attrs _mm_srl_epi8(__m128i a, __m128i b) {
    return _mm_and_si128(_mm_srl_epi16(a, b), _mm_set1_epi8(0xFFU >> _mm_cvtsi128_si64(b))); // 5-7 cycles ...
}

// _mm_mullo_epi16: 3
static_inline __m128i _mm_mullo_epi8(__m128i a, __m128i b) {
//    return _mm_or_si128( // ~12 cycles
//        _mm_srli_epi16(_mm_slli_epi16(_mm_mullo_epi16(a, b), 8), 8),
//        _mm_slli_epi16(_mm_mullo_epi16(_mm_srli_epi16(a, 8), _mm_srli_epi16(b, 8)), 8)
//    );

    return _mm_unpacklo_epi8( // ~10 cycles, assuming *set* gets translated to a fast load ..
        _mm_shuffle_epi8(
            _mm_mullo_epi16(a, b),
            _mm_set_epi8(0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 14, 12, 10, 8, 6, 4, 2, 0)
        ),
        _mm_shuffle_epi8(
            _mm_mullo_epi16(_mm_srli_epi16(a, 8), _mm_srli_epi16(b, 8)),
            _mm_set_epi8(0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 14, 12, 10, 8, 6, 4, 2, 0)
        )
    );  // ^^^^^ slight faster with -Ox when x > 0 slower with -O0
}

static_inline __m128i  intrsc_attrs _mm_mullo_epi64(__m128i a, __m128i b) { // ~14 cycles ...
    return _mm_add_epi64(
        _mm_slli_epi64(
            _mm_add_epi64(
                _mm_mul_epu32(b, _mm_shuffle_epi32(a, _MM_SHUFFLE(2, 3, 0, 1))),
                _mm_mul_epu32(a, _mm_shuffle_epi32(b, _MM_SHUFFLE(2, 3, 0, 1)))
            ),
            32
        ),
        _mm_mul_epu32(a, b)
    );
}


static const struct {
    const struct {const __m128i intgl;} types;

    const struct {
        const struct {
            sse_intgl_load_t  load, load_align;

            sse_intgl_store_t store, store_align;

            sse_bin_intgl_t and, or, xor;

            void *const brdcst[9];
            void *const set[9];
            void *const get[9];


            sse_bin_intgl_t
                add[9],
                mul[9],
                lshft[9],
                rshft_lgcl[9],
                rshft_arith[9],
                eq[9]
            ;

            sse_bin_intgl_imm_int_t
                lshft_imm[9],
                rshft_lgcl_imm[9],
                rshft_arith_imm[9]
            ;

            sse_unr_intgl_t
                extd_sign_bit[9];

            int (*const msb_mask)(__m128i);

            __m128i (*const setzeros)();
        } intgl;
    } ops;
} sse = {
    .ops.intgl = { // unsigned ...
        .load_align     = &_mm_load_si128,
        .store_align    = &_mm_store_si128,
        .load           = &_mm_lddqu_si128,
        .store          = &_mm_storeu_si128,

        .xor            = &_mm_xor_si128,
        .or             = &_mm_or_si128,
        .and            = &_mm_and_si128,

        .setzeros       = &_mm_setzero_si128,

        .brdcst = {
            [1] = (void *)&_mm_set1_epi8,
            [2] = (void *)&_mm_set1_epi16,
            [4] = (void *)&_mm_set1_epi32,
            [8] = (void *)&_mm_set1_epi64x
        },

        .get = {
            [1] = &_mm_extrt_epi8,
            [2] = &_mm_extract_epi16,
            [4] = &_mm_extrt_epi32,
            [8] = &_mm_extrt_epi64
        },

        .set = {
            [2] = &_mm_set_epi16,
            [4] = &_mm_set_epi32,
            [8] = &_mm_set_epi64x
        },

        .rshft_lgcl_imm = {
            [1] = &_mm_srli_epi8,
            [2] = &_mm_srli_epi16,
            [4] = &_mm_srli_epi32,
            [8] = &_mm_srli_epi64
        },

        .rshft_lgcl = {
            [1] = &_mm_srl_epi8,
            [2] = &_mm_srl_epi16,
            [4] = &_mm_srl_epi32,
            [8] = &_mm_srl_epi64
        },

        .lshft_imm = {
            [2] = &_mm_slli_epi16,
            [4] = &_mm_slli_epi32,
            [8] = &_mm_slli_epi64
        },

        .lshft = {
            [2] = &_mm_sll_epi16,
            [4] = &_mm_sll_epi32,
            [8] = &_mm_sll_epi64
        },

        .extd_sign_bit = {
            [4] = &_mm_is_neg_epi32,
            [8] = &_mm_is_neg_epi64
        },

        .add = {
            [4] = &_mm_add_epi32,
            [8] = &_mm_add_epi64
        },

        .mul = {
            [1] = &_mm_mullo_epi8,
            [2] = &_mm_mullo_epi16,
            [4] = &_mm_mullo_epi32,
            [8] = &_mm_mullo_epi64
        },

        .eq[1] = &_mm_cmpeq_epi8,

        .msb_mask = &_mm_movemask_epi8
    }
};



#endif