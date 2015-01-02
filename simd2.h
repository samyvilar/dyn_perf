#ifndef __SIMD_2__
#define __SIMD_2__

#include "comp_utils.h"

typedef union {
    unsigned long long  uint64[1];
    unsigned int        uint32[2];
    unsigned short      uint16[4];
    unsigned char       uint8 [8];
} simd64i;

typedef union {
    double doubl[1];
    unsigned long long  uint64[1];
    float singl[2];
}simd64d;

typedef union {
    unsigned long long  uint64[1];
    double doubl[1];
    float singl[2];
}simd64;

typedef void (*const simd64i_intgl_store_t)   (simd64i *, simd64i);

typedef simd64i (*const simd64i_intgl_load_t) (const simd64i *);

typedef simd64i (*const simd64i_bin_intgl_t)  (simd64i, simd64i);
typedef simd64i (*const simd64i_unr_intgl_t)  (simd64i);
typedef simd64i (*const simd64i_nulr_intgl_t) ();

typedef simd64i (*const simd64i_bin_intgl_imm_int_t)(simd64i, const int);

static_inline simd64i  simd64i_load(const simd64i *src) {return (simd64i){.uint64[0] = src->uint64[0]};}
static_inline void     simd64i_store(simd64i *dest, simd64i value) {dest->uint64[0] = value.uint64[0]; }

static_inline simd64i  simd64i_and(simd64i a, simd64i b) {return (simd64i){.uint64[0] = a.uint64[0] & b.uint64[0]};}
static_inline simd64i  simd64i_xor(simd64i a, simd64i b) {return (simd64i){.uint64[0] = a.uint64[0] ^ b.uint64[0]};}
static_inline simd64i  simd64i_or(simd64i a, simd64i b)  {return (simd64i){.uint64[0] = a.uint64[0] | b.uint64[0]};}

static_inline simd64i simd64i_set1_epi8(char a)       {
//    return (simd64i){.uint64[0] = (unsigned long long)a * 72340172838076673ULL};
    return (simd64i){
        .uint8[0] = a, .uint8[1] = a, .uint8[2] = a, .uint8[3] = a,
        .uint8[4] = a, .uint8[5] = a, .uint8[6] = a, .uint8[7] = a
    };
}
static_inline simd64i simd64i_set1_epi16(short a)     {
//    return (simd64i){.uint64[0] = (unsigned long long)a * 281479271743489ULL};
    return (simd64i){.uint16[0] = a, .uint16[1] = a, .uint16[2] = a, .uint16[3] = a};
}
static_inline simd64i simd64i_set1_epi32(int a)       {
    return (simd64i){.uint32[0] = a, .uint32[1] = a};
}
static_inline simd64i simd64i_set1_epi64(long long a) {return (simd64i){.uint64[0] = a};}

static_inline simd64i simd64i_set_epi8(char a7, char a6, char a5, char a4, char a3, char a2, char a1, char a0) {
    return (simd64i){
        .uint8[0] = a0,
        .uint8[1] = a1,
        .uint8[2] = a2,
        .uint8[3] = a3,
        .uint8[4] = a4,
        .uint8[5] = a5,
        .uint8[6] = a6,
        .uint8[7] = a7
    };
}
static_inline simd64i simd64i_set_epi16(short a3, short a2, short a1, short a0) {
    return (simd64i){.uint16[0] = a0, .uint16[1] = a1, .uint16[2] = a2, .uint16[3] = a3};
}
static_inline simd64i simd64i_set_epi32(int a1, int a0) {return (simd64i){.uint32[0] = a0, .uint32[1] = a1};}
static_inline simd64i simd64i_set_epi64(long long a0)   {return (simd64i){.uint64[0] = a0};}

static_inline char      simd64i_extrt_epi8 (simd64i a, const int id) {return a.uint8 [id]; }
static_inline short     simd64i_extrt_epi16(simd64i a, const int id) {return a.uint16[id];}
static_inline int       simd64i_extrt_epi32(simd64i a, const int id) {return a.uint32[id];}
static_inline long long simd64i_extrt_epi64(simd64i a, const int id) {return a.uint64[id];}


static_inline char      simd64i_cvtsimd64i_si8 (simd64i a)  {return a.uint8 [0];}
static_inline short     simd64i_cvtsimd64i_si16(simd64i a)  {return a.uint16[0];}
static_inline int       simd64i_cvtsimd64i_si32(simd64i a)  {return a.uint32[0];}
static_inline long long simd64i_cvtsimd64i_si64(simd64i a)  {return a.uint64[0];}

#define __simd64i_epi8_bin_templ(a, b, opr_symbol)   \
(simd64i){                                           \
    .uint8[0] = a.uint8[0] opr_symbol b.uint8[0],    \
    .uint8[1] = a.uint8[1] opr_symbol b.uint8[1],    \
    .uint8[2] = a.uint8[2] opr_symbol b.uint8[2],    \
    .uint8[3] = a.uint8[3] opr_symbol b.uint8[3],    \
    .uint8[4] = a.uint8[4] opr_symbol b.uint8[4],    \
    .uint8[5] = a.uint8[5] opr_symbol b.uint8[5],    \
    .uint8[6] = a.uint8[6] opr_symbol b.uint8[6],    \
    .uint8[7] = a.uint8[7] opr_symbol b.uint8[7]     \
}

#define __simd64i_epi16_bin_templ(a, b, opr_symbol)  \
(simd64i){                                           \
    .uint16[0] = a.uint16[0] opr_symbol b.uint16[0], \
    .uint16[1] = a.uint16[1] opr_symbol b.uint16[1], \
    .uint16[2] = a.uint16[2] opr_symbol b.uint16[2], \
    .uint16[3] = a.uint16[3] opr_symbol b.uint16[3]  \
}
#define __simd64i_epi32_bin_templ(a, b, opr_symbol)  \
(simd64i){                                           \
    .uint32[0] = a.uint32[0] opr_symbol b.uint32[0], \
    .uint32[1] = a.uint32[1] opr_symbol b.uint32[1]  \
}
#define __simd64i_epi64_bin_templ(a, b, opr_symbol)  \
(simd64i){.uint64[0] = a.uint64[0] opr_symbol b.uint64[0]}


static_inline simd64i simd64_add_epi8(simd64i a, simd64i b)   {return __simd64i_epi8_bin_templ (a, b, +);}
static_inline simd64i simd64_add_epi16(simd64i a, simd64i b)  {return __simd64i_epi16_bin_templ(a, b, +);}
static_inline simd64i simd64_add_epi32(simd64i a, simd64i b)  {return __simd64i_epi32_bin_templ(a, b, +);}
static_inline simd64i simd64_add_epi64(simd64i a, simd64i b)  {return __simd64i_epi64_bin_templ(a, b, +);}

static_inline simd64i simd64_sub_epi8(simd64i a, simd64i b)   {return __simd64i_epi8_bin_templ (a, b, -);}
static_inline simd64i simd64_sub_epi16(simd64i a, simd64i b)  {return __simd64i_epi16_bin_templ(a, b, -);}
static_inline simd64i simd64_sub_epi32(simd64i a, simd64i b)  {return __simd64i_epi32_bin_templ(a, b, -);}
static_inline simd64i simd64_sub_epi64(simd64i a, simd64i b)  {return __simd64i_epi64_bin_templ(a, b, -);}

static_inline simd64i simd64_mullo_epi8(simd64i a, simd64i b)  {return __simd64i_epi8_bin_templ (a, b, *);}
static_inline simd64i simd64_mullo_epi16(simd64i a, simd64i b) {return __simd64i_epi16_bin_templ(a, b, *);}
static_inline simd64i simd64_mullo_epi32(simd64i a, simd64i b) {return __simd64i_epi32_bin_templ(a, b, *);}
static_inline simd64i simd64_mullo_epi64(simd64i a, simd64i b) {return __simd64i_epi64_bin_templ(a, b, *);}

static_inline simd64i simd64_eq_epi8(simd64i a, simd64i b) {
    return (simd64i){
        .uint8[0] = (unsigned char)(a.uint8[0] != b.uint8[0]) - (unsigned char)1,
        .uint8[1] = (unsigned char)(a.uint8[1] != b.uint8[1]) - (unsigned char)1,
        .uint8[2] = (unsigned char)(a.uint8[2] != b.uint8[2]) - (unsigned char)1,
        .uint8[3] = (unsigned char)(a.uint8[3] != b.uint8[3]) - (unsigned char)1,
        .uint8[4] = (unsigned char)(a.uint8[4] != b.uint8[4]) - (unsigned char)1,
        .uint8[5] = (unsigned char)(a.uint8[5] != b.uint8[5]) - (unsigned char)1,
        .uint8[6] = (unsigned char)(a.uint8[6] != b.uint8[6]) - (unsigned char)1,
        .uint8[7] = (unsigned char)(a.uint8[7] != b.uint8[7]) - (unsigned char)1
    };
}
static_inline simd64i simd64_eq_epi16(simd64i a, simd64i b) {
    return (simd64i){
        .uint16[0] = (unsigned short)(a.uint16[0] != b.uint16[0]) - (unsigned short)1,
        .uint16[1] = (unsigned short)(a.uint16[1] != b.uint16[1]) - (unsigned short)1,
        .uint16[2] = (unsigned short)(a.uint16[2] != b.uint16[2]) - (unsigned short)1,
        .uint16[3] = (unsigned short)(a.uint16[3] != b.uint16[3]) - (unsigned short)1
    };
}
static_inline simd64i simd64_eq_epi32(simd64i a, simd64i b) {
    return (simd64i){
        .uint32[0] = (unsigned)(a.uint32[0] != b.uint32[0]) - 1U,
        .uint32[1] = (unsigned)(a.uint32[1] != b.uint32[1]) - 1U
    };
}
static_inline simd64i simd64_eq_epi64(simd64i a, simd64i b) {
    return (simd64i){.uint64[0] = (unsigned long long)(a.uint64[0] != b.uint64[0]) - 1ULL};
}



#define __simd64i_shft_epi8(a, cast, mag, shft_opr)    \
(simd64i){                                             \
    .uint8[0] = cast(a.uint8[0]) shft_opr mag,         \
    .uint8[1] = cast(a.uint8[1]) shft_opr mag,         \
    .uint8[2] = cast(a.uint8[2]) shft_opr mag,         \
    .uint8[3] = cast(a.uint8[3]) shft_opr mag,         \
    .uint8[4] = cast(a.uint8[4]) shft_opr mag,         \
    .uint8[5] = cast(a.uint8[5]) shft_opr mag,         \
    .uint8[6] = cast(a.uint8[6]) shft_opr mag,         \
    .uint8[7] = cast(a.uint8[7]) shft_opr mag          \
}
#define __simd64i_shft_epi16(a, cast, mag, shft_opr)   \
(simd64i){                                             \
    .uint16[0] = cast(a.uint16[0]) shft_opr mag,       \
    .uint16[1] = cast(a.uint16[1]) shft_opr mag,       \
    .uint16[2] = cast(a.uint16[2]) shft_opr mag,       \
    .uint16[3] = cast(a.uint16[3]) shft_opr mag        \
}
#define __simd64i_shft_epi32(a, cast, mag, shft_opr)   \
(simd64i){                                             \
    .uint32[0] = cast(a.uint32[0]) shft_opr mag,       \
    .uint32[1] = cast(a.uint32[1]) shft_opr mag        \
}
#define __simd64i_shft_epi64(a, cast, mag, shft_opr)   \
(simd64i){.uint64[0] = cast(a.uint64[0]) shft_opr mag}

#define __identity(args...) args

static_inline simd64i simd64_shftleft_epi8 (simd64i a, simd64i b) {return __simd64i_shft_epi8 (a, __identity, b.uint64[0], <<);}
static_inline simd64i simd64_shftleft_epi16(simd64i a, simd64i b) {return __simd64i_shft_epi16(a, __identity, b.uint64[0], <<);}
static_inline simd64i simd64_shftleft_epi32(simd64i a, simd64i b) {return __simd64i_shft_epi32(a, __identity, b.uint64[0], <<);}
static_inline simd64i simd64_shftleft_epi64(simd64i a, simd64i b) {return __simd64i_shft_epi64(a, __identity, b.uint64[0], <<);}

static_inline simd64i simd64_shftleft_imm_epi8 (simd64i a, const int b) {return __simd64i_shft_epi8 (a, __identity, b, <<);}
static_inline simd64i simd64_shftleft_imm_epi16(simd64i a, const int b) {return __simd64i_shft_epi16(a, __identity, b, <<);}
static_inline simd64i simd64_shftleft_imm_epi32(simd64i a, const int b) {return __simd64i_shft_epi32(a, __identity, b, <<);}
static_inline simd64i simd64_shftleft_imm_epi64(simd64i a, const int b) {return __simd64i_shft_epi64(a, __identity, b, <<);}

static_inline simd64i simd64_shftrghtlgcl_epi8 (simd64i a, simd64i b) {return __simd64i_shft_epi8 (a, __identity, b.uint64[0], >>);}
static_inline simd64i simd64_shftrghtlgcl_epi16(simd64i a, simd64i b) {return __simd64i_shft_epi16(a, __identity, b.uint64[0], >>);}
static_inline simd64i simd64_shftrghtlgcl_epi32(simd64i a, simd64i b) {return __simd64i_shft_epi32(a, __identity, b.uint64[0], >>);}
static_inline simd64i simd64_shftrghtlgcl_epi64(simd64i a, simd64i b) {return __simd64i_shft_epi64(a, __identity, b.uint64[0], >>);}

static_inline simd64i simd64_shftrghtlgcl_imm_epi8 (simd64i a, const int b) {return __simd64i_shft_epi8 (a, __identity, b, >>);}
static_inline simd64i simd64_shftrghtlgcl_imm_epi16(simd64i a, const int b) {return __simd64i_shft_epi16(a, __identity, b, >>);}
static_inline simd64i simd64_shftrghtlgcl_imm_epi32(simd64i a, const int b) {return __simd64i_shft_epi32(a, __identity, b, >>);}
static_inline simd64i simd64_shftrghtlgcl_imm_epi64(simd64i a, const int b) {return __simd64i_shft_epi64(a, __identity, b, >>);}

#define __simd64i_cast_singed_char(a)       ((char)(a))
#define __simd64i_cast_singed_short(a)      ((short)(a))
#define __simd64i_cast_singed_int(a)        ((int)(a))
#define __simd64i_cast_singed_long_long(a)  ((long long)(a))
static_inline simd64i simd64_shftrghtarith_epi8 (simd64i a, simd64i b) {return __simd64i_shft_epi8 (a, __simd64i_cast_singed_char,      b.uint64[0], >>);}
static_inline simd64i simd64_shftrghtarith_epi16(simd64i a, simd64i b) {return __simd64i_shft_epi16(a, __simd64i_cast_singed_short,     b.uint64[0], >>);}
static_inline simd64i simd64_shftrghtarith_epi32(simd64i a, simd64i b) {return __simd64i_shft_epi32(a, __simd64i_cast_singed_int,       b.uint64[0], >>);}
static_inline simd64i simd64_shftrghtarith_epi64(simd64i a, simd64i b) {return __simd64i_shft_epi64(a, __simd64i_cast_singed_long_long, b.uint64[0], >>);}

static_inline simd64i simd64_shftrghtarith_imm_epi8 (simd64i a, const int b) {return __simd64i_shft_epi8 (a, __simd64i_cast_singed_char,      b, >>);}
static_inline simd64i simd64_shftrghtarith_imm_epi16(simd64i a, const int b) {return __simd64i_shft_epi16(a, __simd64i_cast_singed_short,     b, >>);}
static_inline simd64i simd64_shftrghtarith_imm_epi32(simd64i a, const int b) {return __simd64i_shft_epi32(a, __simd64i_cast_singed_int,       b, >>);}
static_inline simd64i simd64_shftrghtarith_imm_epi64(simd64i a, const int b) {return __simd64i_shft_epi64(a, __simd64i_cast_singed_long_long, b, >>);}


static_inline simd64i __simd64_extd_sign_bit_epi8(simd64i a) {
    return simd64_shftrghtarith_imm_epi8(a, (int)(bit_sz(a.uint8[0]) - 1));
}
static_inline simd64i __simd64_extd_sign_bit_epi16(simd64i a) {
    return simd64_shftrghtarith_imm_epi16(a, (int)(bit_sz(a.uint16[0]) - 1));
}

static_inline simd64i __simd64_extd_sign_bit_epi32(simd64i a) {
    return simd64_shftrghtarith_imm_epi32(a, (int)(bit_sz(a.uint32[0]) - 1));
}

static_inline simd64i __simd64_extd_sign_bit_epi64(simd64i a) {
    return simd64_shftrghtarith_imm_epi64(a, (int)(bit_sz(a.uint64[0]) - 1));
}

static_inline simd64i simd64_setzeros_si64() {return (simd64i){.uint64[0] = 0};}


static const struct {
    const struct {const simd64i intgl;} types;

    const struct {
        const struct {
            simd64i_intgl_load_t  load, load_align;

            simd64i_intgl_store_t store, store_align;

            simd64i_bin_intgl_t and, or, xor;

            void *const brdcst[9];
            void *const init[9];
            void *const get[9], *const get_first[9];


            simd64i_bin_intgl_t
                add[9],
                sub[9],
                mul[9],
                lshft[9],
                rshft_lgcl[9],
                rshft_arith[9],
                eq[9]
            ;


            simd64i_bin_intgl_imm_int_t
                lshft_imm[9],
                rshft_lgcl_imm[9],
                rshft_arith_imm[9]
            ;

            simd64i_unr_intgl_t
                extd_sign_bit[9];

            int (*const msb_mask)(simd64i);

            const simd64i_nulr_intgl_t setzeros;
        } intgl;
    } ops;
} simd = {
    .ops.intgl = {
        .load        = &simd64i_load,
        .load_align  = &simd64i_load,
        .store       = &simd64i_store,
        .store_align = &simd64i_store,

        .brdcst = {[1] = &simd64i_set1_epi8, [2] = &simd64i_set1_epi16, [4] = &simd64i_set1_epi32, [8] = &simd64i_set1_epi64},

        .init   = {[1] = &simd64i_set_epi8,  [2] = &simd64i_set_epi16,  [4] = &simd64i_set_epi32,  [8] = &simd64i_set_epi64},

        .get_first = {
            [1] = &simd64i_cvtsimd64i_si8,
            [2] = &simd64i_cvtsimd64i_si16,
            [4] = &simd64i_cvtsimd64i_si32,
            [8] = &simd64i_cvtsimd64i_si64
        },

        .get = {
            [1] = &simd64i_extrt_epi8,
            [2] = &simd64i_extrt_epi16,
            [4] = &simd64i_extrt_epi32,
            [8] = &simd64i_extrt_epi64
        },

        .and = &simd64i_and,
        .or  = &simd64i_or,
        .xor = &simd64i_xor,

        .eq     = {[1] = &simd64_eq_epi8,       [2] = &simd64_eq_epi16,         [4] = &simd64_eq_epi32,         [8] = &simd64_eq_epi64},
        .add    = {[1] = &simd64_add_epi8,      [2] = &simd64_add_epi16,        [4] = &simd64_add_epi32,        [8] = &simd64_add_epi64},
        .sub    = {[1] = &simd64_sub_epi8,      [2] = &simd64_sub_epi16,        [4] = &simd64_sub_epi32,        [8] = &simd64_sub_epi64},
        .mul    = {[1] = &simd64_mullo_epi8,    [2] = &simd64_mullo_epi16,      [4] = &simd64_mullo_epi32,      [8] = &simd64_mullo_epi64},
        .lshft  = {[1] = &simd64_shftleft_epi8, [2] = &simd64_shftleft_epi16,   [4] = &simd64_shftleft_epi32,   [8] = &simd64_shftleft_epi64},
        .lshft_imm = {
            [1] = &simd64_shftleft_imm_epi8,
            [2] = &simd64_shftleft_imm_epi16,
            [4] = &simd64_shftleft_imm_epi32,
            [8] = &simd64_shftleft_imm_epi64
        },

        .rshft_lgcl = {
            [1] = &simd64_shftrghtlgcl_epi8,
            [2] = &simd64_shftrghtlgcl_epi16,
            [4] = &simd64_shftrghtlgcl_epi32,
            [8] = &simd64_shftrghtlgcl_epi64
        },
        .rshft_lgcl_imm = {
            [1] = &simd64_shftrghtlgcl_imm_epi8,
            [2] = &simd64_shftrghtlgcl_imm_epi16,
            [4] = &simd64_shftrghtlgcl_imm_epi32,
            [8] = &simd64_shftrghtlgcl_imm_epi64
        },

        .rshft_arith = {
            [1] = &simd64_shftrghtarith_epi8, [2] = &simd64_shftrghtarith_epi16,   [4] = &simd64_shftrghtarith_epi32,   [8] = &simd64_shftrghtarith_epi64
        },

        .rshft_arith_imm = {
            [1] = &simd64_shftrghtarith_imm_epi8, [2] = &simd64_shftrghtarith_imm_epi16,   [4] = &simd64_shftrghtarith_imm_epi32,   [8] = &simd64_shftrghtarith_imm_epi64
        },
        .extd_sign_bit = {
            [1] = &__simd64_extd_sign_bit_epi8,
            [2] = &__simd64_extd_sign_bit_epi16,
            [4] = &__simd64_extd_sign_bit_epi32,
            [8] = &__simd64_extd_sign_bit_epi64
        },

        .setzeros = &simd64_setzeros_si64
    }
};

#endif