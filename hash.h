#ifndef __HASH__
#define __HASH__

#include <assert.h>

#include "mt_rand.h"
#include "scalrs.h"
#include "comp_utils.h"

#include "entry.h"
//#include "vect.h"
//#include "sse2.h"


#define MAX_PRIME_8         (unsigned char)255  // <<<< largest 8 bit prime number ...
#define MAX_SQRT_PRIME_8    (unsigned char)13   // <<<< largest 8 bit prime that is less than sqrt(2**8)


#define MAX_PRIME_16        (unsigned short)65521 // <<<< largest 16 bit prime ...
#define MAX_SQRT_PRIME_16   (unsigned short)255 // <<<< largest 16 bit prime that is less than sqrt(2**16)

#define MAX_PRIME_32        4294967291U // <<<< largest 32 bit prime
#define MAX_SQRT_PRIME_32   65413U      // <<<< largest prime that is less than the sqrt(2^32)

#define MAX_PRIME_64        18446744073709551557LLU // <<<< largest 64 bit prime
#define MAX_SQRT_PRIME_64   4294967291LLU           // <<<< largest prime that is less than the sqrt(2^64)

//#define uint128bit_t (unsigned long long[2])
//#define MAX_PRIME_128       (uint128bit_t){0xFFFFFFFFFFFFFF61L, 0XFFFFFFFFFFFFFFFF}
////      ^^^^^^^^^^^^^ largest 128 bit prime (340282366920938463463374607431768211297)
//#define MAX_SQRT_PRIME_128  (uint128bit_t){18446744073709551557LLU, 0}
////      ^^^^^^^^^^^^^^^^^^ largest prime that is less than sqrt(2^64)

#define max_prime(expr_or_type) scalr_switch_uint(expr_or_type, MAX_PRIME_64, MAX_PRIME_32, MAX_PRIME_16, MAX_PRIME_8, (void)0)
//  ^^^^^^^^^^^^^^^^^^ returns the largest prime that is less than (2**bit_sz(expr_or_type))

#define max_sqrt_prime(expr_or_type) \
    scalr_switch_uint(expr_or_type, MAX_SQRT_PRIME_64, MAX_SQRT_PRIME_32, MAX_SQRT_PRIME_16, MAX_SQRT_PRIME_8)
// ^^^^^^^^^^^^^^^^^^^^^^^^^ returns the largest prime that is less than the sqrt(bit_sz(expr_or_type)) ...
//                           required for universal hash functions ....


#ifndef rand_64
#   define rand_64 mt_rand_vect_64
#endif

#ifndef rand_32
#   define rand_32 mt_rand_vect_32
#endif
#ifndef rand_16
#   define rand_16 rand_32
#endif
#ifndef rand_8
#   define rand_8  rand_32
#endif

//({*(unsigned long *)(int[2]){rand(), rand()};})

#define hash_rand_coef(type_or_expr)    \
    ((scalr_switch_oblvs_sign_intgl(    \
        type_or_expr,                   \
        rand_64(),                      \
        rand_32(),                      \
        rand_16(),                      \
        rand_8(),                       \
        (void)0                         \
    ) /*% max_prime(type_or_expr)*/) | (_t(type_or_expr))1)

#define hash_rand_coef_packd(tp_or_expr) ({             \
    typedef vect_lrgst_intgl_type oprn_t;               \
    typedef _t(tp_or_expr)  memb_t;                     \
                                                        \
    oprn_t (*const brdcst)(_t(memb_t))                  \
        = vect.lrgst.intgl.ops->brdcst[_s(memb_t)];     \
                                                        \
    vect.lrgst.intgl.ops->or(                           \
        scalr_switch_oblvs_sign_intgl(                  \
            memb_t,                                     \
            mt_rand_packd_temprd_bits_64(),             \
            mt_rand_packd_temprd_bits_32(),             \
            mt_rand_packd_temprd_bits_32(),             \
            mt_rand_packd_temprd_bits_32(),             \
            (void)0                                     \
        ),                                              \
        brdcst(1)                                       \
    ); })


static_inline _t(entry_null->key) hash_univ_pow2(
    _t(entry_null->key) id,
    const _t(entry_null->key) coef,
    const unsigned irlvnt_bits
) {
    id *= coef;
    switch (_s(id)) {
        case _s(char):      return (unsigned char)id        >> irlvnt_bits;
        case _s(short):     return (unsigned short)id       >> irlvnt_bits;
        case _s(int):       return (unsigned int)id         >> irlvnt_bits;
        case _s(long long): return (unsigned long long)id   >> irlvnt_bits;
        default: assert(0);
    }
    assert(0);
}

#endif
