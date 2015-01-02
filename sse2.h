
#ifndef __SSE_H__
#define __SSE_H__

#include <immintrin.h>
#include <stdio.h>

#include "comp_utils.h"
#include "avx2.h"

#ifndef __SSE2__
    typedef union {
        unsigned char  uint8[16];
        unsigned short uint16[8];
        unsigned int   uint32[4];
        unsigned long  uint64[2];
    } __m128i;
#endif

typedef void (*const sse_intgl_store_t)   (__m128i *, __m128i);

typedef __m128i (*const sse_intgl_load_t) (const __m128i *);

typedef __m128i (*const sse_bin_intgl_t)  (__m128i, __m128i);
typedef __m128i (*const sse_unr_intgl_t)  (__m128i);
typedef __m128i (*const sse_nulr_intgl_t) ();

typedef __m128i (*const sse_bin_intgl_imm_int_t)(__m128i, const int);

typedef __m128i (*const sse_intgl_gather_t)(const void *, __m128i, const int);


#ifdef __SSE2__

static_inline __m128i _mm_identity_si128(__m128i a) {return a;}


static_inline __m128i ___mm_cvtepu8_epi16(__m128i a) {
    return
#   if defined(__SSE4_1__)
        _mm_cvtepu8_epi16(a) // ~1 cycles ...
#   elif defined(__SSSE3__)
        _mm_shuffle_epi8(a, _mm_set_epi8(0x80, 7, 0x80, 6, 0x80, 5, 0x80, 4, 0x80, 3, 0x80, 2, 0x80, 1, 0x80, 0))
#   else
        _mm_unpacklo_epi8(a, _mm_setzero_si128()) // ~2 cycles ..
#   endif
    ;
}
static_inline __m128i ___mm_cvtepu8_epi32(__m128i a) {
#   if defined(__SSE4_1__)
        return _mm_cvtepu8_epi32(a); // ~1 cycle ...
#   elif defined(__SSSE3__)
        return _mm_shuffle_epi8(
            a,
            _mm_set_epi8(
                0x80, 0x80, 0x80, 3,
                0x80, 0x80, 0x80, 2,
                0x80, 0x80, 0x80, 1,
                0x80, 0x80, 0x80, 0
            )
        );
#   else
        a = _mm_unpacklo_epi8(a, a); // a0, a0, a1, a1, a2, a2, a3, a3, ....
        return _mm_srli_epi32(_mm_unpacklo_epi16(a, a), 24); // ~ 3 cycles ...
#   endif
}
static_inline __m128i ___mm_cvtepu8_epi64(__m128i a) {
    return
#   if defined(__SSE4_1__)
        _mm_cvtepu8_epi64(a) // ~1 cycle ...
#   elif defined(__SSSE3__)
        _mm_shuffle_epi8(
            a,
            _mm_set_epi8(
                0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 1,
                0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0
            )
        )
#   else
        _mm_and_si128(_mm_unpacklo_epi64(a, _mm_srli_si128(a, 1)), _mm_set1_epi64x(0xFF))
#   endif
    ;
}


static_inline __m128i ___mm_cvtepu16_epi8(__m128i a) {
    return
#   if defined(__SSSE3__)
        _mm_shuffle_epi8(a, _mm_set_epi8(0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 14, 12, 10, 8, 6, 4, 2, 0))
#   else
        _mm_packus_epi16(_mm_and_si128(a, _mm_set1_epi16(0xFFU)), _mm_setzero_si128()) // ~ 4 cls ...
#   endif
    ;
}
static_inline __m128i ___mm_cvtepu16_epi32(__m128i a) {
    return
#   if defined(__SSE4_1__)
        _mm_cvtepu16_epi32(a)
#   elif defined(__SSSE3__)
        _mm_shuffle_epi8(
            a,
            _mm_set_epi8(
                0x80, 0x80, 7, 6,
                0x80, 0x80, 5, 4,
                0x80, 0x80, 3, 2,
                0x80, 0x80, 1, 0
            )
        )
#   else
        _mm_unpacklo_epi16(a, _mm_setzero_si128())
#   endif
    ;
}
static_inline __m128i ___mm_cvtepu16_epi64(__m128i a) {
    return
#   if   defined(__SSE4_1__)
        _mm_cvtepu16_epi64(a)
#   elif defined(__SSSE3__)
        _mm_shuffle_epi8(a, _mm_set_epi8(0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 3, 2, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 1, 0))
#   else
        _mm_srli_epi64( // 3 cycles ...
            _mm_shuffle_epi32(
                _mm_shufflelo_epi16(a, _MM_SHUFFLE(1, 1, 0, 0)),
                _MM_SHUFFLE(1, 1, 0, 0)
            ),
            48
        )
#   endif
        ;
}


static_inline __m128i ___mm_cvtepu32_epi8(__m128i a) { // truncate 4 32 bits to 4 8 bits ...
#   if defined(__SSSE3__)
        return _mm_shuffle_epi8(
            a,
            _mm_set_epi8(0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 12, 8, 4, 0)
        );
#   else
        a = _mm_and_si128(a, _mm_set1_epi32(0xFFU)); // 0, 0, 0, a12, 0, 0, 0, a8, 0, 0, 0, a4, 0, 0, 0, a0
        a = _mm_packs_epi32(a, a); // 0, a12, 0, a8, 0, a4, 0, a0, 0, a12, 0, a8, 0, a4, 0, a0
        return _mm_srli_si128(_mm_packus_epi16(a, a), 12); // a12, a8, a4, a0, a12, a8, a4, a0, a12, a8, a4, a0
        // ~5 cycles ...
#   endif
}

static_inline __m128i ___mm_cvtepu32_epi16(__m128i a) { // truncate 4 32 bits to 16 bits, zero out upper
    return
#   if defined(__SSSE3__)
        _mm_shuffle_epi8(
            a,
            _mm_set_epi8(0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 13, 12, 9, 8, 5, 4, 1, 0)
        )
        // ^^^^^^ 1-2 cycles
#   else
    _mm_move_epi64(
        _mm_shuffle_epi32(
            _mm_shufflehi_epi16(
                _mm_shufflelo_epi16(a, _MM_SHUFFLE(3, 2, 2, 0)),
                _MM_SHUFFLE(3, 2, 2, 0)
            ),
            _MM_SHUFFLE(3, 1, 2, 0)
        )
    ) // ^^^^^ 4 cycles ...
#endif
    ;
}
static_inline __m128i ___mm_cvtepu32_epi64(__m128i a) { // convert initial 2 32 bits to 2 64 bits ...
    return _mm_srli_epi64(_mm_shuffle_epi32(a, _MM_SHUFFLE(1, 2, 0, 0)), 32); // ~ 2 cycles ..
}


static_inline __m128i ___mm_cvtepu64_epi8(__m128i a) {
    return
#   if defined(__SSSE3__)
        _mm_shuffle_epi8(
            a,
            _mm_set_epi8(0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 8, 0)
        )
#   else
        _mm_srli_si128(_mm_slli_si128(_mm_unpacklo_epi8(a, _mm_srli_si128(a, 8)), 14), 14) // ~4
#   endif
    ;
}
static_inline __m128i ___mm_cvtepu64_epi16(__m128i a) {
    return
#   if defined(__SSSE3__)
        _mm_shuffle_epi8(
            a,
            _mm_set_epi8(
                0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
                0x80, 0x80, 0x80, 0x80, 9, 8, 1, 0
            )
        )
#   else
        _mm_srli_si128(
            _mm_shufflehi_epi16(
                _mm_shuffle_epi32(a, _MM_SHUFFLE(2, 0, 1, 0)),
                _MM_SHUFFLE(2, 0, 1, 0)
            ),
            12
        ) // ^^^^^^ ~3 cls
#   endif
        ;
}

static_inline __m128i ___mm_cvtepu64_epi32(__m128i a) {
    return _mm_srli_si128(_mm_shuffle_epi32(a, _MM_SHUFFLE(2, 0, 1, 0)), 8); // ~2 cls
}



static_inline __m128i intrsc_attrs _mm_is_neg_epi8 (__m128i a) { return _mm_cmplt_epi8(a, _mm_setzero_si128()); } // ~2 cycles
static_inline __m128i intrsc_attrs _mm_is_neg_epi16(__m128i a) { return _mm_srai_epi16(a, 16); } // ~1 cycles
static_inline __m128i intrsc_attrs _mm_is_neg_epi32(__m128i a) { return _mm_srai_epi32(a, 32); } // ~1 cycles
static_inline __m128i intrsc_attrs _mm_is_neg_epi64(__m128i a) { return _mm_srai_epi32(_mm_shuffle_epi32(a, _MM_SHUFFLE(3, 3, 1, 1)), 32);} // ~2 cycles

static_inline void sse_error_not_implemented(__m128i _, ...) {
    printf("ERROR: SSE operation not implemented!");
    exit(-1);
}

#if !defined(__SSE4_1__) && !defined(__INTEL_COMPILER)

    static_inline int _mm_extrt_epi8(__m128i a, unsigned id) { // ~4 cycles, 6-8% slower vs native instruction ...
        if (id & 1)
            return _mm_extract_epi16(a, id >> 1) >> 8; // odd index ...

        return _mm_extract_epi16(a, id >> 1) & 0xFF; // even index ...
        // ^^^^^^^^^^^^^^^^ 4 cycles ...
    }

    static_inline int intrsc_attrs _mm_extrt_epi32(__m128i a, const int imm) {
        switch (imm & 3) {
            case 0: return _mm_cvtsi128_si32(a);
            case 1: return _mm_cvtsi128_si32(_mm_srli_si128(a, 4));
            case 2: return _mm_cvtsi128_si32(_mm_srli_si128(a, 8));
            case 3: return _mm_cvtsi128_si32(_mm_srli_si128(a, 12));
        }
        __builtin_unreachable();
    }

    static_inline long long intrsc_attrs _mm_extrt_epi64(__m128i a, const int imm) {
        if (imm & 1)
            return _mm_cvtsi128_si64(_mm_srli_si128(a, 8));
        return _mm_cvtsi128_si64(a);
    }

    static_inline __m128i intrsc_attrs _mm_mullo_epi32(__m128i a, __m128i b) { // ~11 cycles
        return _mm_unpacklo_epi32(
            _mm_shuffle_epi32(_mm_mul_epu32(a, b), _MM_SHUFFLE(0, 0, 2, 0)),
            _mm_shuffle_epi32(_mm_mul_epu32(_mm_srli_si128(a, 4), _mm_srli_si128(b, 4)), _MM_SHUFFLE(0, 0, 2, 0))
        );
    }

    static_inline int intrsc_attrs _mm_test_all_ones(__m128i a) {
        return _mm_movemask_epi8(_mm_cmpeq_epi32(a, _mm_cmpeq_epi32(a, a))) == 0xFFFF;
    }

    static_inline int intrsc_attrs _mm_test_all_zeros(__m128i a, __m128i mask) {
        return _mm_movemask_epi8(_mm_cmpeq_epi32(_mm_and_si128(a, mask), _mm_andnot_si128(a, a))) == 0xFFFF;
    }

    static_inline __m128i intrsc_attrs _mm_cmpeq_epi64(__m128i a, __m128i b) { // ~ 3 cycles ...
        a = _mm_cmpeq_epi32(a, b);
        return _mm_and_si128(a, _mm_shuffle_epi32(a, _MM_SHUFFLE(2, 3, 0, 1)));
    }

#else
#   ifdef __INTEL_COMPILER
#       define _mm_extrt_epi8  _mm_extract_epi8
#       define _mm_extrt_epi32 _mm_extract_epi32
#       define _mm_extrt_epi64 _mm_extract_epi64
#   else
        static_inline int       intrsc_attrs _mm_extrt_epi8 (__m128i a, const int imm) {return _mm_extract_epi8 (a, imm);}
        static_inline int       intrsc_attrs _mm_extrt_epi32(__m128i a, const int imm) {return _mm_extract_epi32(a, imm);}
        static_inline long long intrsc_attrs _mm_extrt_epi64(__m128i a, const int imm) {return _mm_extract_epi64(a, imm);}
#   endif
#endif

static_inline __m128i intrsc_attrs _mm_srli_epi8(__m128i a, const int b) { // 2-3 cycles ...
    return _mm_and_si128(_mm_srli_epi16(a, b), _mm_set1_epi8(0xFFU >> b));
}

static_inline __m128i intrsc_attrs _mm_srl_epi8(__m128i a, __m128i b) { // 5-7 cycles ...
    return _mm_and_si128(_mm_srl_epi16(a, b), _mm_set1_epi8(0xFFU >> _mm_cvtsi128_si64(b)));
}

// _mm_mullo_epi16: 3
static_inline __m128i _mm_mullo_epi8(__m128i a, __m128i b) {
    return
#       ifdef __SSSE3__
            _mm_unpacklo_epi8( // ~11 cycles, assuming both *set* get translated to a single fast load (which it does at least with 'clang -Ofast')
                _mm_shuffle_epi8(
                    _mm_mullo_epi16(a, b),
                    _mm_set_epi8(0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 14, 12, 10, 8, 6, 4, 2, 0)
                ),
                _mm_shuffle_epi8(
                    _mm_mullo_epi16(_mm_srli_epi16(a, 8), _mm_srli_epi16(b, 8)),
                    _mm_set_epi8(0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 14, 12, 10, 8, 6, 4, 2, 0)
                )
            )
#       else
            _mm_or_si128( // ~12 cycles
                _mm_srli_epi16(_mm_slli_epi16(_mm_mullo_epi16(a, b), 8), 8),
                _mm_slli_epi16(_mm_mullo_epi16(_mm_srli_epi16(a, 8), _mm_srli_epi16(b, 8)), 8)
            )
#       endif
    ;
}

static_inline __m128i _mm_slli_epi8(__m128i a, const int _mag_imm) { // ~ 2-3 cycles ...
    return _mm_and_si128(_mm_slli_epi16(a, _mag_imm), _mm_set1_epi8((unsigned char)(0xFFU << (_mag_imm & 15))));
}

static_inline __m128i _mm_sll_epi8(__m128i a, __m128i b) { // (4-6 cycles)
    return _mm_and_si128(_mm_sll_epi16(a, b), _mm_set1_epi8(0xFFU << _mm_cvtsi128_si64(b)));
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

static_inline short _mm_cvtsi128_si16 (__m128i a) {return _mm_cvtsi128_si32(a) & 0xFFFFU;}
static_inline char  _mm_cvtsi128_si8  (__m128i a) {return _mm_cvtsi128_si32(a) & 0xFFU;  }

static_inline int _mm_iszeros(__m128i a) {
    return
#   if defined(__SSE4_1__)
        _mm_test_all_zeros(a, a);
#   else
        _mm_cvtsi128_si64(_mm_or_si128(a, _mm_srli_si128(a, 8))) == 0
#   endif
    ;
}

//static_inline __m128i  ___normalize_i8(__m128i indices, const int scale) {
//    static sse_bin_intgl_imm_int_t slli = &_mm_slli_epi8;
//    static sse_bin_intgl_t
//        add = &_mm_add_epi8,
//        sub = &_mm_sub_epi8,
//        mul = &_mm_mullo_epi8;
//    static __m128i (*const brdcst)(int) = &_mm_set1_epi8;
//
//    switch (scale) {
//        case 0: return _mm_setzero_si128();
//        case 1: return indices;
//
//        case 2: return add(indices, indices);
//        case 3: return add(add(indices, indices), indices);
//        case 4: return slli(indices, 2);
//        case 5: return add(slli(indices, 2), indices);
//        case 6: return add(slli(indices, 2), slli(indices, 1));
//        case 7: return sub(slli(indices, 4), indices);
//        case 8: return slli(indices, 3);
//
//        default: return mul(indices, brdcst(scale));
//    }
//    __builtin_unreachable();
//}


static_inline __m128i  ___normalize_i16(__m128i indices, const int scale) {
    static sse_bin_intgl_imm_int_t slli = &_mm_slli_epi16;
    static sse_bin_intgl_t
        add = &_mm_add_epi16,
        sub = &_mm_sub_epi16,
        mul = &_mm_mullo_epi16;
    static __m128i (*const brdcst)(short) = &_mm_set1_epi16;

    switch (scale) {
        case 0: return _mm_setzero_si128();
        case 1: return indices;

        case 2: return add(indices, indices);
        case 3: return add(add(indices, indices), indices);
        case 4: return slli(indices, 2);
        case 5: return add(slli(indices, 2), indices);
        case 6: return add(slli(indices, 2), slli(indices, 1));
        case 7: return sub(slli(indices, 4), indices);
        case 8: return slli(indices, 3);

        default: return mul(indices, brdcst(scale));
    }
    __builtin_unreachable();
}


static_inline __m128i  ___normalize_i32(__m128i indices, const int scale) {
    static sse_bin_intgl_imm_int_t slli = &_mm_slli_epi32;
    static sse_bin_intgl_t
        add = &_mm_add_epi32,
        sub = &_mm_sub_epi32,
        mul = &_mm_mullo_epi32;
    static __m128i (*const brdcst)(int) = &_mm_set1_epi32;

    switch (scale) {
        case 0: return _mm_setzero_si128();
        case 1: return indices;

        case 2: return add(indices, indices);
        case 3: return add(add(indices, indices), indices);
        case 4: return slli(indices, 2);
        case 5: return add(slli(indices, 2), indices);
        case 6: return add(slli(indices, 2), slli(indices, 1));
        case 7: return sub(slli(indices, 4), indices);
        case 8: return slli(indices, 3);

        default: return mul(indices, brdcst(scale));
    }
    __builtin_unreachable();
}

static_inline __m128i  ___normalize_i64(__m128i indices, const int scale) {
    static sse_bin_intgl_imm_int_t slli = &_mm_slli_epi64;
    static sse_bin_intgl_t
        add = &_mm_add_epi64,
        sub = &_mm_sub_epi64,
        mul = &_mm_mullo_epi64;
    static const _t(&_mm_set1_epi64x) brdcst = &_mm_set1_epi64x;

    switch (scale) {
        case 0: return _mm_setzero_si128();
        case 1: return indices;

        case 2: return add(indices, indices);
        case 3: return add(add(indices, indices), indices);
        case 4: return slli(indices, 2);
        case 5: return add(slli(indices, 2), indices);
        case 6: return add(slli(indices, 2), slli(indices, 1));
        case 7: return sub(slli(indices, 4), indices);
        case 8: return slli(indices, 3);

        default: return mul(indices, brdcst(scale));
    }
    __builtin_unreachable();
}

#ifndef __GATHER__
#define __GATHER__(base, indices, scale, gather_func)({         \
    _t(gather_func(NULL, indices, 0)) dest;                     \
    switch (scale) {                                            \
        case 0: dest = gather_func(base, indices, 0);  break ;  \
        case 1: dest = gather_func(base, indices, 1);  break ;  \
        case 2: dest = gather_func(base, indices, 2);  break ;  \
        case 3: dest = gather_func(base, indices, 3);  break ;  \
        case 4: dest = gather_func(base, indices, 4);  break ;  \
        case 5: dest = gather_func(base, indices, 5);  break ;  \
        case 6: dest = gather_func(base, indices, 6);  break ;  \
        case 7: dest = gather_func(base, indices, 7);  break ;  \
        case 8: dest = gather_func(base, indices, 8);  break ;  \
        default: assert(0); \
    } dest; })
#endif

static_inline __m128i ___mm_i64gather_epi64(const void *base, __m128i indices, const int scale) {
#   ifndef __AVX2__
    indices = _mm_add_epi64(_mm_set1_epi64x((unsigned long long)base), ___normalize_i64(indices, scale));
    return _mm_set_epi64x(
        *(unsigned long long *)_mm_extrt_epi64(indices, 1),
        *(unsigned long long *)_mm_extrt_epi64(indices, 0)
    );
#   else
    return __GATHER__(base, indices, scale, _mm_i64gather_epi64);
#   endif
}

static_inline __m128i ___mm_i64gather_epi32(const void *base, __m128i indices, const int scale) {
#   ifndef __AVX2__
    return ___mm_cvtepu64_epi32(___mm_i64gather_epi64(base, indices, scale));
#   else
    return __GATHER__(base, indices, scale, _mm_i64gather_epi32);
#   endif
}

static_inline __m128i ___mm_i64gather_epi16(const void *base, __m128i indices, const int scale) {
    return ___mm_cvtepu64_epi16(___mm_i64gather_epi64(base, indices, scale)); // gather 2 16 bit umbers using 2 64 bit indices
}



static_inline __m128i ___mm_i32gather_epi64(const void *base, __m128i indices, const int scale) {
#ifndef __AVX2__
    switch (_s(void *)) {
        case 4:
            indices = _mm_add_epi64(_mm_set1_epi32((unsigned)base), ___normalize_i32(indices, scale));
            return _mm_set_epi64x(
                *(unsigned long long *) (size_t)_mm_extrt_epi32(indices, 1),
                *(unsigned long long *) (size_t)_mm_extrt_epi32(indices, 0)
            );
        case 8:
            indices = _mm_add_epi64(
                _mm_set1_epi64x((unsigned long long)base),
                ___mm_cvtepu32_epi64(___normalize_i32(indices, scale))
            );
            return _mm_set_epi64x(
                *(unsigned long long *) _mm_extrt_epi64(indices, 1),
                *(unsigned long long *) _mm_extrt_epi64(indices, 0)
            );
        default: assert(0);
    }
    __builtin_unreachable();
#else
    return __GATHER__(base, indices, scale, _mm_i32gather_epi64);
#endif
}

static_inline __m128i ___mm_i32gather_epi32(const void *base, __m128i indices, const int scale) {
#ifndef __AVX2__
    indices = ___normalize_i32(indices, scale);

    switch (_s(void *)) {
        case 4:
            indices = _mm_add_epi32(_mm_set1_epi32((unsigned int)base), indices);
            return _mm_set_epi32(
                *(int *)(size_t)_mm_extrt_epi32(indices, 3),
                *(int *)(size_t)_mm_extrt_epi32(indices, 2),
                *(int *)(size_t)_mm_extrt_epi32(indices, 1),
                *(int *)(size_t)_mm_extrt_epi32(indices, 0)
            );

        case 8: {
            __m128i
                low = _mm_add_epi64(
                    _mm_set1_epi64x((unsigned long long) base),
                    ___mm_cvtepu32_epi64(indices)
                ),
                high = _mm_add_epi64(
                    _mm_set1_epi64x((unsigned long long) base),
                    _mm_shuffle_epi32((_mm_srli_si128(indices, 8)), _MM_SHUFFLE(3, 1, 3, 0))
                )
            ;
            return _mm_set_epi32(
                *(int *) _mm_extrt_epi64(high, 1),
                *(int *) _mm_extrt_epi64(high, 0),
                *(int *) _mm_extrt_epi64(low, 1),
                *(int *) _mm_extrt_epi64(low, 0)
            );
        }
        default: assert(0);
    }
    __builtin_unreachable();
#else
    return __GATHER__(base, indices, scale, _mm_i32gather_epi32);
#endif
}

static_inline __m128i ___mm_i32gather_epi16(const void *base, __m128i indices, const int scale) { // ~ 9-10 cycles ..
    return ___mm_cvtepu32_epi16(___mm_i32gather_epi32(base, indices, scale)); // gather 4 16 bit umbers using 4 32 bit indices
}



static_inline __m128i ___mm_i16gather_epi64(const void *base, __m128i indices, const int scale) {
    return ___mm_i64gather_epi64(base, ___mm_cvtepu16_epi64(indices), scale);
}

static_inline __m128i ___mm_i16gather_epi32(const void *base, __m128i indices, const int scale) {
    return ___mm_i32gather_epi32(base, ___mm_cvtepu16_epi32(indices), scale);
}

static_inline __m128i ___mm_i16gather_epi16(const void *base, __m128i indices, const int scale) {  // ~ 16 cycles ...
    indices = ___normalize_i16(indices, scale);
    return _mm_or_si128(
        _mm_and_si128(
            ___mm_i32gather_epi32(base, _mm_and_si128(indices, _mm_set1_epi32(0xFFFFU)), 1),
            _mm_set1_epi32(0xFFFFU)
        ),
        _mm_slli_epi32(___mm_i32gather_epi32(base, _mm_srli_epi32(indices, 16), 1), 16)
    );
}


static_inline __m128i ___mm_sllv_epi64(__m128i a, __m128i b) {
#   ifndef __AVX2__
    return _mm_set_epi64x(
        _mm_extrt_epi64(a, 1) << _mm_extrt_epi64(b, 1),
        _mm_extrt_epi64(a, 0) << _mm_extrt_epi64(b, 0)
    );
#   else
    return _mm_sllv_epi64(a, b);
#   endif
}

static_inline __m128i ___mm_sllv_epi32(__m128i a, __m128i b) {
#   ifndef __AVX2__
    return _mm_set_epi32(
        _mm_extrt_epi32(a, 3) << _mm_extract_epi16(b, 6),
        _mm_extrt_epi32(a, 2) << _mm_extract_epi16(b, 4),
        _mm_extrt_epi32(a, 1) << _mm_extract_epi16(b, 2),
        _mm_cvtsi128_si32(a) << _mm_cvtsi128_si32(b)
    );
#   else
    return _mm_sllv_epi32(a, b);
#   endif
}


static_inline __m128i ___mm_sllv_epi8(__m128i a, __m128i b) {
    size_t index;
    for (index = 0; index < 16; index++)
        ((unsigned char *)&a)[index] <<= ((unsigned char *)&b)[index];
    return a;
}

static_inline __m128i ___mm_sllv_epi16(__m128i a, __m128i b) {
#   ifndef __AVX2__
    return _mm_set_epi16(
        (unsigned short)(_mm_extract_epi16(a, 7) << _mm_extract_epi16(b, 7)),
        (unsigned short)(_mm_extract_epi16(a, 6) << _mm_extract_epi16(b, 6)),
        (unsigned short)(_mm_extract_epi16(a, 5) << _mm_extract_epi16(b, 5)),
        (unsigned short)(_mm_extract_epi16(a, 4) << _mm_extract_epi16(b, 4)),
        (unsigned short)(_mm_extract_epi16(a, 3) << _mm_extract_epi16(b, 3)),
        (unsigned short)(_mm_extract_epi16(a, 2) << _mm_extract_epi16(b, 2)),
        (unsigned short)(_mm_extract_epi16(a, 1) << _mm_extract_epi16(b, 1)),
        (unsigned short)(_mm_extract_epi16(a, 0) << _mm_cvtsi128_si32(b))
    );
#   else
    return _mm_sllv_epi32(a, b);
#   endif
}


#endif


static const struct {
    const struct {const __m128i intgl;} types;

    const struct {
        const struct {
            sse_intgl_load_t  load, load_align;

            sse_intgl_store_t store, store_align;

            sse_bin_intgl_t and, or, xor;

            void *const brdcst[9];
            void *const init[9];
            void *const get[9], *const get_first[9];


            sse_bin_intgl_t
                add[9],
                mul[9],
                lshft[9],
                lshft_varbl[9], // variable shifts ....
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
                extd_sign_bit[9],
                cvtepu[9][9];


            sse_intgl_gather_t
                gather[9][9];


            int (*const msb_mask)(__m128i);

            int (*const iszeros)(__m128i);

            const sse_nulr_intgl_t setzeros;
        } intgl;
    } ops;
} sse
#ifdef __SSE2__
    = {
    .ops.intgl = {
        .load_align     = &_mm_load_si128,
        .store_align    = &_mm_store_si128,
        .load           = &_mm_lddqu_si128,
        .store          = &_mm_storeu_si128,

        .xor            = &_mm_xor_si128,
        .or             = &_mm_or_si128,
        .and            = &_mm_and_si128,

        .setzeros       = &_mm_setzero_si128,

        .brdcst = {
            [1] = &_mm_set1_epi8,
            [2] = &_mm_set1_epi16,
            [4] = &_mm_set1_epi32,
            [8] = &_mm_set1_epi64x
        },

        .get = {
            [1] = &_mm_extrt_epi8,
            [2] = &_mm_extract_epi16,
            [4] = &_mm_extrt_epi32,
            [8] = &_mm_extrt_epi64
        },

        .get_first = {
            [1] = &_mm_cvtsi128_si8,
            [2] = &_mm_cvtsi128_si16,
            [4] = &_mm_cvtsi128_si32,
            [8] = &_mm_cvtsi128_si64
        },

        .init = {
            [1] = &_mm_set_epi8,
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
            [1] = &_mm_slli_epi8,
            [2] = &_mm_slli_epi16,
            [4] = &_mm_slli_epi32,
            [8] = &_mm_slli_epi64
        },

        .lshft = {
            [1] = &_mm_sll_epi8,
            [2] = &_mm_sll_epi16,
            [4] = &_mm_sll_epi32,
            [8] = &_mm_sll_epi64
        },

        .lshft_varbl = {
            [1] = &___mm_sllv_epi8,
            [2] = &___mm_sllv_epi16,
            [4] = &___mm_sllv_epi32,
            [8] = &___mm_sllv_epi64
        },

        .extd_sign_bit = {
            [1] = &_mm_is_neg_epi8,
            [2] = &_mm_is_neg_epi16,
            [4] = &_mm_is_neg_epi32,
            [8] = &_mm_is_neg_epi64
        },

        .add = {
            [1] = &_mm_add_epi8,
            [2] = &_mm_add_epi16,
            [4] = &_mm_add_epi32,
            [8] = &_mm_add_epi64
        },

        .mul = {
            [1] = &_mm_mullo_epi8,
            [2] = &_mm_mullo_epi16,
            [4] = &_mm_mullo_epi32,
            [8] = &_mm_mullo_epi64
        },

        .eq = {
            [1] = &_mm_cmpeq_epi8,
            [2] = &_mm_cmpeq_epi16,
            [4] = &_mm_cmpeq_epi32,
            [8] = &_mm_cmpeq_epi64
        },

        .cvtepu = {
            [1] = {
                [1] = &_mm_identity_si128,

                [2] = &___mm_cvtepu8_epi16,
                [4] = &___mm_cvtepu8_epi32,
                [8] = &___mm_cvtepu8_epi64
            },

            [2] = {
                [2] = &_mm_identity_si128,

                [1] = &___mm_cvtepu16_epi8,
                [4] = &___mm_cvtepu16_epi32,
                [8] = &___mm_cvtepu16_epi64
            },

            [4] = {
                [4] = &_mm_identity_si128,

                [1] = &___mm_cvtepu32_epi8,
                [2] = &___mm_cvtepu32_epi16,
                [8] = &___mm_cvtepu32_epi64
            },

            [8] = {
                [8] = &_mm_identity_si128,

                [1] = &___mm_cvtepu64_epi8,
                [2] = &___mm_cvtepu64_epi16,
                [4] = &___mm_cvtepu64_epi32
            },
        },

        .gather = {
            [2] = {
                [2] = &___mm_i16gather_epi16,
                [4] = &___mm_i32gather_epi16,
                [8] = &___mm_i64gather_epi16
            },
            [4] = {
                [2] = &___mm_i16gather_epi32,
                [4] = &___mm_i32gather_epi32,
                [8] = &___mm_i64gather_epi32
            },
            [8] = {
                [2] = &___mm_i16gather_epi64,
                [4] = &___mm_i32gather_epi64,
                [8] = &___mm_i64gather_epi64
            }
        },

        .msb_mask = &_mm_movemask_epi8,
        .iszeros = &_mm_iszeros
    }
}
#endif
;
#endif