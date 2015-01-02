
#ifndef __AVX_2__
#define __AVX_2__

#include <immintrin.h>

#include "comp_utils.h"

#ifndef __AVX2__
    typedef union {
        unsigned long   uint64[4];
        unsigned int    uint32[8];
        unsigned short  uint16[16];
        unsigned char   uint8 [32];
    } __m256i;
#else
static_inline __m256i _mm256_identity_si256(__m256i a) {return a;}

// cvt instructions ...
static_inline __m256i _mm256_cvtepi32_epu16(__m256i a) { // ~4-5 cycles truncate 8 32 bits to 8 16 bits
    return _mm256_permute4x64_epi64( // 3 cycles ...
        _mm256_shuffle_epi8( // ~1-2 cycles ..
            a,
            _mm256_set_epi8(
                0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 13, 12, 9, 8, 5, 4, 1, 0,
                0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 13, 12, 9, 8, 5, 4, 1, 0
            )
        ),
        _MM_SHUFFLE(3, 1, 2, 0)
    );
}

static_inline __m256i ___mm256_cvtepu8_epi16(__m256i a) {return _mm256_cvtepu8_epi16(_mm256_castsi256_si128(a));}
static_inline __m256i ___mm256_cvtepu8_epi32(__m256i a) {return _mm256_cvtepu8_epi32(_mm256_castsi256_si128(a));}
static_inline __m256i ___mm256_cvtepu8_epi64(__m256i a) {return _mm256_cvtepu8_epi64(_mm256_castsi256_si128(a));}

static_inline __m256i ___mm256_cvtepu16_epi8(__m256i a) { // ~5 cycles ...
    return _mm256_permute4x64_epi64(
        _mm256_shuffle_epi8(
            a,
            _mm256_set_epi8(
                0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 14, 12, 10, 8, 6, 4, 2, 0,
                0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 14, 12, 10, 8, 6, 4, 2, 0
            )
        ),
        _MM_SHUFFLE(3, 3, 2, 0)
    );
}

static_inline __m256i ___mm256_cvtepu16_epi32(__m256i a) {return _mm256_cvtepu16_epi32(_mm256_castsi256_si128(a));}
static_inline __m256i ___mm256_cvtepu16_epi64(__m256i a) {return _mm256_cvtepu16_epi64(_mm256_castsi256_si128(a));}

static_inline __m256i ___mm256_cvtepu32_epi8(__m256i a) { // ~6
    return _mm256_permutevar8x32_epi32(
        _mm256_shuffle_epi8(
            a,
            _mm256_set_epi8(
                0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 12, 8, 4, 0,
                0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 12, 8, 4, 0
            )
        ),
        _mm256_set_epi32(5, 5, 5, 5, 3, 3, 4, 0)
    );

}
static_inline __m256i ___mm256_cvtepu32_epi16(__m256i a) { // ~5 cycles ...
    return _mm256_permute4x64_epi64(
        _mm256_shuffle_epi8(
            a,
            _mm256_set_epi8(
                0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 14, 12, 9, 8, 5, 4, 1, 0,
                0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 14, 12, 9, 8, 5, 4, 1, 0
            )
        ),
        _MM_SHUFFLE(3, 3, 2, 0)
    );
}
static_inline __m256i ___mm256_cvtepu32_epi64(__m256i a) {return _mm256_cvtepu32_epi64(_mm256_castsi256_si128(a));}


static_inline __m256i ___mm256_cvtepu64_epi8(__m256i a) { // ~7
    return _mm256_shufflelo_epi16(
        _mm256_permutevar8x32_epi32(
            _mm256_shuffle_epi8(
                a,
                _mm256_set_epi8(
                    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 8, 0,
                    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 8, 0
                )
            ),
            _mm256_set_epi32(7, 6, 5, 5, 3, 2, 4, 0)
        ),
        _MM_SHUFFLE(3, 3, 2, 0)
    );
}
static_inline __m256i ___mm256_cvtepu64_epi16(__m256i a) { // ~6
    return _mm256_permutevar8x32_epi32(
        _mm256_shuffle_epi8(
            a,
            _mm256_set_epi8(
                0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 9, 8, 1, 0,
                0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 9, 8, 1, 0
            )
        ),
        _mm256_set_epi32(7, 6, 5, 5, 3, 2, 4, 0)
    );
}
static_inline __m256i ___mm256_cvtepu64_epi32(__m256i a) { // ~5
    return _mm256_permutevar8x32_epi32(_mm256_slli_epi64(a, 32), _mm256_set_epi32(6, 6, 6, 6, 7, 5, 3, 1));
}

// gather instructions ...

#define __GATHER__(base, indices, scale, gather_func)({         \
    _t(gather_func(NULL, indices, 0)) dest;                     \
    switch (scale) {                                            \
        /*case 0: dest = gather_func(base, indices, 0);  break ;*/  \
        case 1: dest = gather_func(base, indices, 1);  break ;  \
        case 2: dest = gather_func(base, indices, 2);  break ;  \
        /*case 3: dest = gather_func(base, indices, 3);  break ; */ \
        case 4: dest = gather_func(base, indices, 4);  break ;  \
        /*case 5: dest = gather_func(base, indices, 5);  break ; */ \
        /*case 6: dest = gather_func(base, indices, 6);  break ;  */\
        /*case 7: dest = gather_func(base, indices, 7);  break ;  */\
        case 8: dest = gather_func(base, indices, 8);  break ;  \
        default: assert(0); \
    } dest; })

static_inline __m256i ___mm256_i32gather_epi32(const void *base, __m256i indices, const int scale) {
    return __GATHER__(base, indices, scale, _mm256_i32gather_epi32);
}
static_inline __m256i ___mm256_i32gather_epi64(const void *base, __m256i indices, const int scale) {
    return __GATHER__(base, _mm256_castsi256_si128(indices), scale, _mm256_i32gather_epi64);
}
static_inline __m256i ___mm256_i64gather_epi32(const void *base, __m256i indices, const int scale) {
    return _mm256_castsi128_si256(__GATHER__(base, indices, scale, _mm256_i64gather_epi32));
}
static_inline __m256i ___mm256_i64gather_epi64(const void *base, __m256i indices, const int scale) {
    return __GATHER__(base, indices, scale, _mm256_i64gather_epi64);
}

static_inline __m256i ___mm256_i16gather_epi64(const void *base, __m256i indices, const int scale) { // ~9 cycles ...
    return ___mm256_i64gather_epi64(base, _mm256_cvtepu16_epi64(_mm256_castsi256_si128(indices)), scale); // gather 4 64 bit using 4 16 bit indices ...
}

static_inline __m256i ___mm256_i16gather_epi32(const void *base, __m256i indices, const int scale) { // ~9 cycles ...
    return ___mm256_i32gather_epi32(base, _mm256_cvtepu16_epi32(_mm256_castsi256_si128(indices)), scale); // gather 8 32 bits from the low 8 16 bits
}

static_inline __m256i ___mm256_i16gather_epi16(const void *base, __m256i indices, const int scale) {  // ~ 16 cycles ...
    return _mm256_blend_epi16( // gather 16 shorts using 16 short indices ...
        ___mm256_i32gather_epi32(base, _mm256_and_si256(indices, _mm256_set1_epi32(0x0000FFFFU)), scale), // low shorts
        _mm256_slli_epi32(___mm256_i32gather_epi32(base, _mm256_srli_epi32(indices, 16), scale), 16), // high shorts
        0b1010101010101010
    );
}
static_inline __m256i ___mm256_i32gather_epi16(const void *base, __m256i indices, const int scale) { // ~ 9-10 cycles ..
    return _mm256_cvtepi32_epu16(___mm256_i32gather_epi32(base, indices, scale)); // gather 8 shorts using 8 32 bit indices
}
static_inline __m256i ___mm256_i64gather_epi16(const void *base, __m256i indices, const int scale) { // ~6-7 cycles ..
    return _mm256_shuffle_epi8( // gather 4 16 bits from 4 64 bit indices ...
        ___mm256_i64gather_epi32(base, indices, scale),
        _mm256_set_epi8(
            0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 13, 12, 9, 8, 5, 4, 1, 0,
            0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 13, 12, 9, 8, 5, 4, 1, 0
        )
    );
}

static_inline __m256i  ___mm256_i16gather_epi8(const void *base, __m256i indices, const int scale) { // ~20 cycles, gather 16 8 bits using 16 16 bit indices ...
    return ___mm256_cvtepu16_epi8(___mm256_i16gather_epi16(base, indices, scale));
}
static_inline __m256i ___mm256_i32gather_epi8(const void *base, __m256i indices, const int scale) { // 9-10 cycles, gather 8 8 bits using 8 32 bit indices
    return ___mm256_cvtepu32_epi8(___mm256_i32gather_epi32(base, indices, scale));
}
static_inline __m256i ___mm256_i64gather_epi8(const void *base, __m256i indices, const int scale) { // 6-7 cycles ...
    return _mm256_shuffle_epi8( // gather 4 8 bits using 4 64 bit indices ...
        ___mm256_i64gather_epi32(base, indices, scale),
        _mm256_set_epi8(
            0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 12, 8, 4, 0,
            0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 12, 8, 4, 0
        )
    );
}

static_inline __m256i intrsc_attrs _mm256_is_neg_epi8 (__m256i a) { return _mm256_cmpgt_epi8(_mm256_setzero_si256(), a);} // ~2 cycles
static_inline __m256i intrsc_attrs _mm256_is_neg_epi16(__m256i a) { return _mm256_srai_epi16(a, 16); } // ~1 cycles
static_inline __m256i intrsc_attrs _mm256_is_neg_epi32(__m256i a) { return _mm256_srai_epi32(a, 32); } // ~1 cycles
static_inline __m256i intrsc_attrs _mm256_is_neg_epi64(__m256i a) { return _mm256_srai_epi32(_mm256_shuffle_epi32(a, _MM_SHUFFLE(3, 3, 1, 1)), 32);} // ~2 cycles


static_inline int _mm256_extrt_epi8(__m256i a, const int imm) { // ~4-5 cycles ...
#if defined(__ICC) || defined(__INTEL_COMPILER)
    switch (imm) {
        default: assert(0);
        case 24 ... 31: a = _mm256_permute4x64_epi64(a, _MM_SHUFFLE(3, 2, 2, 3)); break ;
        case 16 ... 23: a = _mm256_permute4x64_epi64(a, _MM_SHUFFLE(3, 2, 3, 2)); break ;
        case 8  ... 15: a = _mm256_bsrli_epi128(a, 8); break ;
        case 4  ... 7:  a = _mm256_bsrli_epi128(a, 4); break ;
        case 0  ... 3: break ;
    }
    unsigned src = _mm_cvtsi128_si32(_mm256_castsi256_si128(a));
    switch (imm) {
        case 3: return src >> 24;
        case 2:
        case 1: src >>= (imm << 3);
        case 0: return (unsigned char)src;
    }
    assert(0);
#else
    const register unsigned int src = _mm_extract_epi16(
        (imm > 15) ? _mm256_extracti128_si256(a, 1) : _mm256_castsi256_si128(a),
        (imm & 15) >> 1
    );
    return (imm & 1) ? (src >> 8) : (src & 0xFFU);
#endif
}

static_inline int intrsc_attrs _mm256_extrt_epi16(__m256i a, const int imm) { // ~4 cycles
#if defined(__ICC) || defined(__INTEL_COMPILER)
    switch (imm) {
        default: assert(0);
        case 12 ... 15: a = _mm256_permute4x64_epi64(a, _MM_SHUFFLE(3, 2, 2, 2)); break ;
        case 8  ... 11: a = _mm256_permute4x64_epi64(a, _MM_SHUFFLE(3, 2, 3, 2)); break ;
        case 4  ... 7:  a = _mm256_bsrli_epi128(a, 8); break ;
        case 0  ... 3:  break ;
    }
    unsigned long long src = _mm_cvtsi128_si64x(_mm256_castsi256_si128(a));
    switch (imm) {
        case 3: return src >> 48;
        case 2:
        case 1: src >>= (imm << 4);
        case 0: return (unsigned short)src;
    }
    assert(0);
#else
    return _mm_extract_epi16(
        (imm > 7) ? _mm256_extracti128_si256(a, 1) : _mm256_castsi256_si128(a),
        imm & 7
    );
#endif
}

static_inline int intrsc_attrs _mm256_extrt_epi32(__m256i a, const int imm) { // ~3 cycles
#if defined(__ICC) || defined(__INTEL_COMPILER)
    switch (imm) {
        case 7: a = _mm256_permute4x64_epi64(a, _MM_SHUFFLE(3, 2, 3, 3));
        case 3: a = _mm256_srli_si256(a, 12);
            break ;

        case 5: a = _mm256_permute4x64_epi64(a, _MM_SHUFFLE(3, 2, 3, 2));
        case 1: a = _mm256_srli_si256(a, 4);
            break ;

        case 6: a = _mm256_permute4x64_epi64(a, _MM_SHUFFLE(3, 2, 3, 3));   break ;
        case 4: a = _mm256_permute4x64_epi64(a, _MM_SHUFFLE(3, 2, 3, 2));   break ;
        case 2: a = _mm256_srli_si256(a, 8);                                break ;
        case 0: break ;
    }
    return _mm_cvtsi128_si32(_mm256_castsi256_si128(a));
#else
    return _mm_extract_epi32(
        (imm > 3) ? _mm256_extracti128_si256(a, 1) : _mm256_castsi256_si128(a),
        imm & 3
    );
#endif
}

static_inline unsigned long long intrsc_attrs _mm256_extrt_epi64(__m256i a, const int imm) {
    switch (imm & 3) {
        case 0: return _mm_cvtsi128_si64(_mm256_castsi256_si128(a));
        case 1: return _mm_extract_epi64(_mm256_castsi256_si128(a), 1);
        case 2: return _mm_cvtsi128_si64(_mm256_extracti128_si256(a, 1));
        case 3: return _mm_extract_epi64(_mm256_extracti128_si256(a, 1), 1);
    }
    __builtin_unreachable();
}

static_inline int intrsc_attrs _mm256_test_all_ones(__m256i a) {return _mm256_testc_si256(a, _mm256_cmpeq_epi64(a, a));}

static_inline int intrsc_attrs _mm256_test_all_zeros(__m256i a, __m256i mask) {return _mm256_test_all_zeros(a, mask);}

static_inline char      _mm256_cvtsi256_si8 (__m256i a) {return _mm_cvtsi128_si32(_mm256_castsi256_si128(a)) & 0xFFU;   }
static_inline short     _mm256_cvtsi256_si16(__m256i a) {return _mm_cvtsi128_si32(_mm256_castsi256_si128(a)) & 0xFFFFU; }
static_inline int       _mm256_cvtsi256_si32(__m256i a) {return _mm_cvtsi128_si32(_mm256_castsi256_si128(a));           }
static_inline long long _mm256_cvtsi256_si64(__m256i a) {return _mm_cvtsi128_si64(_mm256_castsi256_si128(a));           }


static_inline __m256i ___mm256_srli_epi64(__m256i a, const int imm) {return _mm256_srli_epi64(a, imm);}
static_inline __m256i ___mm256_srli_epi32(__m256i a, const int imm) {return _mm256_srli_epi32(a, imm);}
static_inline __m256i ___mm256_srli_epi16(__m256i a, const int imm) {return _mm256_srli_epi16(a, imm);}
static_inline __m256i ___mm256_srli_epi8(__m256i a, const int imm) { // ~2-3 cycles ...
    return _mm256_and_si256(_mm256_srli_epi16(a, imm), _mm256_set1_epi8(0xFFU >> imm));
}

// _mm256_broadcastb_epi8: 3
// _mm256_extracti128_si256: 1
// _mm256_srl_epi16: 4
// _mm256_and_si256: 1
// _mm256_srlv_epi32: 2
static_inline __m256i _mm256_lrsft_epi8(__m256i a, __m256i b) { // ~9 cycles ...
    return _mm256_and_si256(
        _mm256_srl_epi16(a, _mm256_castsi256_si128(b)),
        _mm256_set1_epi8(0xFFU >> _mm_cvtsi128_si64(_mm256_castsi256_si128(b)))
    );
}

static_inline __m256i _mm256_lrsft_epi16(__m256i a, __m256i b) {return _mm256_srl_epi16(a, _mm256_castsi256_si128(b));}
static_inline __m256i _mm256_lrsft_epi32(__m256i a, __m256i b) {return _mm256_srl_epi32(a, _mm256_castsi256_si128(b));}
static_inline __m256i _mm256_lrsft_epi64(__m256i a, __m256i b) {return _mm256_srl_epi64(a, _mm256_castsi256_si128(b));}


static_inline __m256i ___mm256_slli_epi8(__m256i a, const int mag) {
    return _mm256_and_si256(_mm256_slli_epi16(a, mag), _mm256_set1_epi8((unsigned char)(0xFFU << mag)));
}
static_inline __m256i ___mm256_slli_epi16(__m256i a, const int mag) {return _mm256_slli_epi16(a, mag);}
static_inline __m256i ___mm256_slli_epi32(__m256i a, const int mag) {return _mm256_slli_epi32(a, mag);}
static_inline __m256i ___mm256_slli_epi64(__m256i a, const int mag) {return _mm256_slli_epi64(a, mag);}

// _mm_cvtsi128_si64: 2
// _mm256_broadcastb_epi8: 3
static_inline __m256i _mm256_llshft_epi8(__m256i a, __m256i b) {
    return _mm256_and_si256(
        _mm256_sll_epi16(a, _mm256_castsi256_si128(b)),
        _mm256_set1_epi8((unsigned char)(0xFF << _mm_cvtsi128_si64(_mm256_castsi256_si128(b)))) // ~ 5 cycles ..
    );
    // ^^^^^^^^^^^^ ~9 cycles ...

//    return _mm256_and_si256( // ~13
//        _mm256_sll_epi16(a, _mm256_castsi256_si128(b)),
//        _mm256_broadcastb_epi8(_mm256_castsi256_si128(_mm256_sll_epi16(_mm_cmpeq_epi16(b, b), b))) // ~8 cycles
//    );
}

static_inline __m256i _mm256_llshft_epi16(__m256i a, __m256i b) {return _mm256_sll_epi16(a, _mm256_castsi256_si128(b));}
static_inline __m256i _mm256_llshft_epi32(__m256i a, __m256i b) {return _mm256_sll_epi32(a, _mm256_castsi256_si128(b));}
static_inline __m256i _mm256_llshft_epi64(__m256i a, __m256i b) {return _mm256_sll_epi64(a, _mm256_castsi256_si128(b));}

static_inline __m256i ___mm256_mullo_epi8(__m256i a, __m256i b) {
//    return _mm256_unpacklo_epi8(
//        _mm256_shuffle_epi8(
//            _mm256_mullo_epi16(a, b),
//            _mm256_set_epi8(
//                0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 14, 12, 10, 8, 6, 4, 2, 0,
//                0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 14, 12, 10, 8, 6, 4, 2, 0
//            )
//        ),
//        _mm256_shuffle_epi8(
//            _mm256_mullo_epi16(_mm256_srli_epi16(a, 8), _mm256_srli_epi16(b, 8)),
//            _mm256_set_epi8(
//                0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 14, 12, 10, 8, 6, 4, 2, 0,
//                0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 14, 12, 10, 8, 6, 4, 2, 0
//            )
//        )
//    ); // ^^^^^^^^^^^^^^^^^^^^^^ 16 cycles ...

    return _mm256_blendv_epi8(
        _mm256_mullo_epi16(a, b),
        _mm256_slli_epi16(_mm256_mullo_epi16(_mm256_srli_epi16(a, 8), _mm256_srli_epi16(b, 8)), 8),
        _mm256_set_epi8(
            0x80, 0, 0x80, 0, 0x80, 0, 0x80, 0, 0x80, 0, 0x80, 0, 0x80, 0, 0x80, 0,
            0x80, 0, 0x80, 0, 0x80, 0, 0x80, 0, 0x80, 0, 0x80, 0, 0x80, 0, 0x80, 0
        )
    ); // ^^^^^^^^^^^^^^^ 15 cycles ...
}

static_inline __m256i ___mm256_mullo_epi16(__m256i a, __m256i b) {return _mm256_mullo_epi16(a, b);}
static_inline __m256i ___mm256_mullo_epi32(__m256i a, __m256i b) {return _mm256_mullo_epi32(a, b);}

static_inline __m256i ___mm256_mullo_epi64(__m256i a, __m256i b) { // ~20 cycles ...
    return _mm256_add_epi64(
        _mm256_mul_epu32(a, b), // low a * low b
        _mm256_slli_epi64(
            _mm256_add_epi64(
                _mm256_mul_epi32(a, _mm256_shuffle_epi32(b, _MM_SHUFFLE(2, 3, 0, 1))), // low a * high b
                _mm256_mul_epi32(b, _mm256_shuffle_epi32(a, _MM_SHUFFLE(2, 3, 0, 1))) // low b * high a
            ),
            32
        )
    );
}

static_inline __m256i ___mm256_sllv_epi64(__m256i a, __m256i b) {return _mm256_sllv_epi64(a, b);}
static_inline __m256i ___mm256_sllv_epi32(__m256i a, __m256i b) {return _mm256_sllv_epi32(a, b);}

static_inline __m256i ___mm256_sllv_epi16(__m256i a, __m256i b) { // ~8 cycles ...
    return _mm256_blend_epi16( // variable shift each 16 bits according to each corresponding 16 bit magnitude
        _mm256_sllv_epi32(a, b),
        _mm256_sllv_epi32(_mm256_and_si256(a, _mm256_set1_epi32(0xFFFF0000)), _mm256_srli_epi32(b, 16)),
        0b1010101010101010
    );
}
static_inline __m256i ___mm256_sllv_epi8(__m256i a, __m256i b) { // ~ 22 cycles ...
    // variable shift each 8 bits according to each corresponding 8 bit magnitude

//    __m256i res0 = _mm256_sllv_epi32(a, b);
//    __m256i res1 = _mm256_sllv_epi32(_mm256_and_si256(a, _mm256_set1_epi32(0x0000FF00)), _mm256_srli_epi32(b, 8));
//    __m256i res2 = _mm256_sllv_epi32(_mm256_and_si256(a, _mm256_set1_epi32(0x00FF0000)), _mm256_srli_epi32(b, 16));
//    __m256i res3 = _mm256_sllv_epi32(_mm256_and_si256(a, _mm256_set1_epi32(0xFF000000)), _mm256_srli_epi32(b, 24));

    return _mm256_blendv_epi8(
        _mm256_blend_epi16(
            _mm256_sllv_epi32(a, b),
            _mm256_sllv_epi32(_mm256_and_si256(a, _mm256_set1_epi32(0x00FF0000)), _mm256_srli_epi32(b, 16)),
            0b1010101010101010
        ),
        _mm256_blend_epi16(
            _mm256_sllv_epi32(_mm256_and_si256(a, _mm256_set1_epi32(0x0000FF00)), _mm256_srli_epi32(b, 8)),
            _mm256_sllv_epi32(_mm256_and_si256(a, _mm256_set1_epi32(0xFF000000)), _mm256_srli_epi32(b, 24)),
            0b1010101010101010
        ),
        _mm256_set_epi8(
            0x80, 0, 0x80, 0, 0x80, 0, 0x80, 0, 0x80, 0, 0x80, 0, 0x80, 0, 0x80, 0,
            0x80, 0, 0x80, 0, 0x80, 0, 0x80, 0, 0x80, 0, 0x80, 0, 0x80, 0, 0x80, 0
        )
    );
}

static_inline int avx_iszeros(__m256i a) {return _mm256_testz_si256(a, a);}

static_inline __m256i ___mm256_lddqu_si256(const __m256i *src) {return _mm256_lddqu_si256(src);}
static_inline __m256i ___mm256_load_si256(const __m256i *src)  {return _mm256_load_si256(src);}

static_inline void ___mm256_store_si256(__m256i *dest, __m256i a) {_mm256_store_si256(dest, a);}
static_inline void ___mm256_storeu_si256(__m256i *dest, __m256i a) {_mm256_storeu_si256(dest, a);}

static_inline __m256i ___mm256_and_si256(__m256i a, __m256i b) {return _mm256_and_si256(a, b);}
static_inline __m256i ___mm256_or_si256(__m256i a, __m256i b) {return _mm256_or_si256(a, b);}
static_inline __m256i ___mm256_xor_si256(__m256i a, __m256i b) {return _mm256_xor_si256(a, b);}

static_inline __m256i ___mm256_add_epi8 (__m256i a, __m256i b) {return _mm256_add_epi8 (a, b);}
static_inline __m256i ___mm256_add_epi16(__m256i a, __m256i b) {return _mm256_add_epi16(a, b);}
static_inline __m256i ___mm256_add_epi32(__m256i a, __m256i b) {return _mm256_add_epi32(a, b);}
static_inline __m256i ___mm256_add_epi64(__m256i a, __m256i b) {return _mm256_add_epi64(a, b);}

static_inline __m256i ___mm256_cmpeq_epi8 (__m256i a, __m256i b) {return _mm256_cmpeq_epi8 (a, b);}
static_inline __m256i ___mm256_cmpeq_epi16(__m256i a, __m256i b) {return _mm256_cmpeq_epi16(a, b);}
static_inline __m256i ___mm256_cmpeq_epi32(__m256i a, __m256i b) {return _mm256_cmpeq_epi32(a, b);}
static_inline __m256i ___mm256_cmpeq_epi64(__m256i a, __m256i b) {return _mm256_cmpeq_epi64(a, b);}

static_inline __m256i ___mm256_set1_epi8(char a)        {return _mm256_set1_epi8(a);}
static_inline __m256i ___mm256_set1_epi16(short a)      {return _mm256_set1_epi16(a);}
static_inline __m256i ___mm256_set1_epi32(int a)        {return _mm256_set1_epi32(a);}
static_inline __m256i ___mm256_set1_epi64x(long long a) {return _mm256_set1_epi64x(a);}

static_inline __m256i ___mm256_set_epi8(char e31, char e30, char e29, char e28, char e27, char e26, char e25, char e24, char e23, char e22, char e21, char e20, char e19, char e18, char e17, char e16, char e15, char e14, char e13, char e12, char e11, char e10, char e9, char e8, char e7, char e6, char e5, char e4, char e3, char e2, char e1, char e0) {
    return _mm256_set_epi8(e31, e30, e29, e28, e27, e26, e25, e24, e23, e22, e21, e20, e19, e18, e17, e16, e15, e14, e13, e12, e11, e10, e9, e8, e7, e6, e5, e4, e3, e2, e1, e0);
}
static_inline __m256i ___mm256_set_epi16 (short e15, short e14, short e13, short e12, short e11, short e10, short e9, short e8, short e7, short e6, short e5, short e4, short e3, short e2, short e1, short e0) {
    return _mm256_set_epi16(e15, e14, e13, e12, e11, e10, e9, e8, e7, e6, e5, e4, e3, e2, e1, e0);
}
static_inline __m256i ___mm256_set_epi32 (int e7, int e6, int e5, int e4, int e3, int e2, int e1, int e0) {
    return _mm256_set_epi32(e7, e6, e5, e4, e3, e2, e1, e0);
}
static_inline __m256i ___mm256_set_epi64x (long long e3, long long e2, long long e1, long long e0) {
    return _mm256_set_epi64x(e3, e2, e1, e0);
}


static_inline __m256i ___mm256_setzero_si256() {return _mm256_setzero_si256();}

static_inline int ___mm256_movemask_epi8(__m256i a) {return _mm256_movemask_epi8(a);}

#endif


typedef void (*const avx_intgl_store_t)   (__m256i *, __m256i);

typedef __m256i (*const avx_intgl_load_t)   (const __m256i *);

typedef __m256i (*const avx_intgl_gather_t) (const void *, __m256i, const int);

typedef __m256i (*const avx_bin_intgl_t)  (__m256i, __m256i);
typedef __m256i (*const avx_unr_intgl_t)  (__m256i);
typedef __m256i (*const avx_nulr_intgl_t) ();

typedef __m256i (*const avx_bin_intgl_imm_int_t)(__m256i, const int);

static const struct {
    const struct {const __m256i intgl;} types;

    const struct {
        const struct {
            avx_bin_intgl_t and;
            avx_bin_intgl_t or;
            avx_bin_intgl_t xor;

            avx_intgl_load_t  load, load_align;

            avx_intgl_store_t store, store_align;

            avx_intgl_gather_t gather[9][9];


            void *const brdcst[9];
            void *const init[9];
            void *const get[9], *const get_first[9];

            avx_bin_intgl_t
                add[9],
                mul[9],
                lshft[9],
                rshft_lgcl[9],
                rshft_arith[9],
                eq[9],

                lshft_varbl[9], // variable shifts ....
                rshft_lgcl_varbl[9],
                rshft_arith_varbl[9]

            ;


            avx_bin_intgl_imm_int_t
                lshft_imm[9],
                rshft_lgcl_imm[9],
                rshft_arith_imm[9]
            ;

            avx_unr_intgl_t
                extd_sign_bit[9],
                cvtepu[9][9];

            int (*const msb_mask)(__m256i);

            const avx_nulr_intgl_t setzeros;

            int (*const iszeros)(__m256i);
        } intgl;
    } ops;
} avx
#ifdef __AVX2__
    = {
        .ops.intgl = {
            .xor            = &___mm256_xor_si256,
            .or             = &___mm256_or_si256,
            .and            = &___mm256_and_si256,

            .load_align     = &___mm256_load_si256,
            .store_align    = &___mm256_store_si256,
            .load           = &___mm256_lddqu_si256,
            .store          = &___mm256_storeu_si256,

            .gather = {
                [1][2] = &___mm256_i16gather_epi8,
                [1][4] = &___mm256_i32gather_epi8,
                [1][8] = &___mm256_i64gather_epi8,

                [2][2] = &___mm256_i16gather_epi16,
                [2][4] = &___mm256_i32gather_epi16,
                [2][8] = &___mm256_i64gather_epi16,

                [4][2] = &___mm256_i16gather_epi32,
                [4][4] = &___mm256_i32gather_epi32,
                [4][8] = &___mm256_i64gather_epi32,

                [8][2] = &___mm256_i16gather_epi64,
                [8][4] = &___mm256_i32gather_epi64,
                [8][8] = &___mm256_i64gather_epi64
            },

            .setzeros       = &___mm256_setzero_si256,

            .brdcst = {
                [1] = &___mm256_set1_epi8,
                [2] = &___mm256_set1_epi16,
                [4] = &___mm256_set1_epi32,
                [8] = &___mm256_set1_epi64x
            },

            .get = {
                [1] = &_mm256_extrt_epi8,
                [2] = &_mm256_extrt_epi16,
                [4] = &_mm256_extrt_epi32,
                [8] = &_mm256_extrt_epi64
            },

            .get_first = {
                [1] = &_mm256_cvtsi256_si8,
                [2] = &_mm256_cvtsi256_si16,
                [4] = &_mm256_cvtsi256_si32,
                [8] = &_mm256_cvtsi256_si64
            },

            .init = {
                [1] = &___mm256_set_epi8,
                [2] = &___mm256_set_epi16,
                [4] = &___mm256_set_epi32,
                [8] = &___mm256_set_epi64x
            },

            .rshft_lgcl_imm = {
                [1] = &___mm256_srli_epi8,
                [2] = &___mm256_srli_epi16,
                [4] = &___mm256_srli_epi32,
                [8] = &___mm256_srli_epi64
            },

            .rshft_lgcl = {
                [1] = &_mm256_lrsft_epi8,
                [2] = &_mm256_lrsft_epi16,
                [4] = &_mm256_lrsft_epi32,
                [8] = &_mm256_lrsft_epi64
            },

            .lshft_imm = {
                [1] = &___mm256_slli_epi8,
                [2] = &___mm256_slli_epi16,
                [4] = &___mm256_slli_epi32,
                [8] = &___mm256_slli_epi64
            },

            .lshft_varbl = {
                [1] = &___mm256_sllv_epi8,
                [2] = &___mm256_sllv_epi16,
                [4] = &___mm256_sllv_epi32,
                [8] = &___mm256_sllv_epi64
            },

            .lshft = {
                [1] = &_mm256_llshft_epi8,
                [2] = &_mm256_llshft_epi16,
                [4] = &_mm256_llshft_epi32,
                [8] = &_mm256_llshft_epi64
            },

            .extd_sign_bit = {
                [1] = &_mm256_is_neg_epi8,
                [2] = &_mm256_is_neg_epi16,
                [4] = &_mm256_is_neg_epi32,
                [8] = &_mm256_is_neg_epi64
            },
            .cvtepu = {
                [1] = {
                    [1] = &_mm256_identity_si256,
                    [2] = &___mm256_cvtepu8_epi16,
                    [4] = &___mm256_cvtepu8_epi32,
                    [8] = &___mm256_cvtepu8_epi64
                },
                [2] = {
                    [1] = &___mm256_cvtepu16_epi8,
                    [2] = &_mm256_identity_si256,
                    [4] = &___mm256_cvtepu16_epi32,
                    [8] = &___mm256_cvtepu16_epi64
                },
                [4] = {
                    [1] = &___mm256_cvtepu32_epi8,
                    [2] = &___mm256_cvtepu32_epi16,
                    [4] = &_mm256_identity_si256,
                    [8] = &___mm256_cvtepu32_epi64
                },
                [8] = {
                    [1] = &___mm256_cvtepu64_epi8,
                    [2] = &___mm256_cvtepu64_epi16,
                    [4] = &___mm256_cvtepu64_epi32,
                    [8] = &_mm256_identity_si256
                }
            },

            .add = {
                [1] = &___mm256_add_epi8,
                [2] = &___mm256_add_epi16,
                [4] = &___mm256_add_epi32,
                [8] = &___mm256_add_epi64
            },

            .mul = {
                [1] = &___mm256_mullo_epi8,
                [2] = &___mm256_mullo_epi16,
                [4] = &___mm256_mullo_epi32,
                [8] = &___mm256_mullo_epi64
            },

            .eq = {
                [1] = &___mm256_cmpeq_epi8,
                [2] = &___mm256_cmpeq_epi16,
                [4] = &___mm256_cmpeq_epi32,
                [8] = &___mm256_cmpeq_epi64
            },

            .msb_mask = &___mm256_movemask_epi8,
            .iszeros = &avx_iszeros
        }
 }
#endif
;
/*
void test_gather() {
    unsigned long buffer[10000];

    unsigned index;
    for (index = 0; index < array_cnt(buffer); buffer[index++] = mt_rand_vect_64()) ;

    void *indices[9] = {[2] = (unsigned short[128]){}, [4] = (unsigned int[128]){}, [8] = (unsigned long[128]){}};
    unsigned memb_sz = 0, indice_sz = 0;

    for (indice_sz = 0; indice_sz < array_cnt(indices); indice_sz++)
        if (indices[indice_sz])
            for (index = 0; index < 128; index++) {
                memcpy(
                    indices[indice_sz] + (index * indice_sz),
                    (unsigned long[1]){[0] = mt_rand_vect_64() % array_cnt(buffer)},
                    indice_sz
                );
            }

    _t(&sse.ops.intgl) ops = &sse.ops.intgl;
    typedef _t(ops->setzeros()) oprn_t;

    for (memb_sz = 0; memb_sz < 9; memb_sz++)
        for (indice_sz = 0; indice_sz < 9; indice_sz++) {
            if (NULL == ops->gather[memb_sz][indice_sz])
                continue ;

            _t(ops->gather[memb_sz][indice_sz]) impl = ops->gather[memb_sz][indice_sz];
            for (index = 0; index < 128; index += _s(oprn_t)/memb_sz)
            {
                void *src = indices[indice_sz] + (index * indice_sz);
                union {oprn_t pckd; unsigned char bytes[_s(oprn_t)];} got = {.pckd = impl(buffer, ops->load(src), memb_sz)};

                _t(got.bytes) exp;
                memset(exp, 0, _s(exp));
                size_t locs[_s(oprn_t)/umax(memb_sz, indice_sz)], curr;
                memset(locs, 0, _s(locs));

                for (curr = 0; curr < array_cnt(locs); curr++)
                    memcpy(&locs[curr], src + (curr * indice_sz), indice_sz);

                for (curr = 0; curr < array_cnt(locs); curr++)
                    memcpy(&exp[curr * memb_sz], (void *)buffer + (locs[curr] * memb_sz), memb_sz);

                for (curr = 0; curr < _s(oprn_t); curr++)
                    if (exp[curr] != got.bytes[curr])
                        printf(
                            "array_cnt(locs): %zi indice_sz: %zi, memb_sz: %zi, failed: index == %zi exp[%zi]: %hhu got[%zi]: %hhu\n",
                            array_cnt(locs), indice_sz, memb_sz, index, curr, exp[curr], curr, got.bytes[curr]
                        ), exit(-1);
            }
        }
}
*/

#endif