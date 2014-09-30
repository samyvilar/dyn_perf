#ifndef __HASH__
#define __HASH__

#include <assert.h>

#include "mt_rand.h"
#include "scalrs.h"


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
//  ^^^^^^^^^^^^^^^^^^ returns the largest prime that is less than (2**bit_size(expr_or_type))

#define max_sqrt_prime(expr_or_type) \
    scalr_switch_uint(expr_or_type, MAX_SQRT_PRIME_64, MAX_SQRT_PRIME_32, MAX_SQRT_PRIME_16, MAX_SQRT_PRIME_8)
// ^^^^^^^^^^^^^^^^^^^^^^^^^ returns the largest prime that is less than the sqrt(bit_size(expr_or_type)) ...
//                           required for universal hash functions ....


#define rand_64 mt_rand_vect_64
#define rand_32 mt_rand_vect_32
#define rand_16 rand_32
#define rand_8  rand_32

#define hash_rand_coef(type_or_expr)    \
    ((scalr_switch_oblvs_sign_intgl(    \
        type_or_expr,                   \
        rand_64(),                      \
        rand_32(),                      \
        rand_16(),                      \
        rand_8(),                       \
        (void)0                         \
    ) % max_prime(type_or_expr)) | (typeof(type_or_expr))1)

//// ** Universal Hash function first degree polynomial ...
//#define _calc_univ_prod_hash(k, a, tbl_size) (((k) * (a)) >> ((typeof(tbl_size))bit_size(tbl_size) - leadn_one_index(tbl_size)))
//#define _calc_univ_prod_pair_wise_hash(k, a, b, tbl_size)   \
//    ((((k) * (a)) + (b)) >> ((typeof(tbl_size))bit_size(tbl_size) - leadn_one_index(tbl_size)))

#define hash_univ_pow2(key, coef, shift_mag) ((key * coef) >> shift_mag)

#define init_hash(self) ({                          \
    (self)->coef = hash_rand_coef((self)->coef);    \
    (self)->shift_mag = bit_size((self)->coef) - bits_leadn_one((self)->length);\
})



//#define rand_params(params, cnt) ({                     \
//    typedef typeof(params[0]) _param_t;                 \
//    _param_t                                            \
//         *_dest = (params)                              \
//        ,_parm_index                                    \
//        ;                                               \
//    for (_parm_index = 0; _parm_index < array_cnt(params); _parm_index++) \
//        _dest[_parm_index] = ((                         \
//            scalr_switch_oblvs_sign_intgl(              \
//                *_dest,                                 \
//                rand_64(),                              \
//                rand_32(),                              \
//                rand_16(),                              \
//                rand_8(),                               \
//                (void)0                                 \
//            ) % max_prime(params[0])) | (_param_t)1);   \
//    _dest; }) // ^^^^^^^^^^^^^^^^^^^^^ must be random odd integer
//              //   that does not exceed the largest prime for its underlying bit size


//    comp_select(                                                                                        \
//        array_cnt(hash_params_coefs(hash_tbl_params(_tbl))) == 1LU,                                     \
//        _calc_univ_prod_hash(_key, hash_params_coef(hash_tbl_params(_tbl), 0), hash_tbl_size(_tbl)),    \
//    comp_select(                                                                                        \
//        array_cnt(hash_params_coefs(hash_tbl_params(_tbl))) == 2LU,         \
//        _calc_univ_prod_pair_wise_hash(                                     \
//            _key,                                                           \
//            hash_params_coef(hash_tbl_params(_tbl), 0),                     \
//            hash_params_second_param(hash_tbl_params(_tbl)),                \
//            hash_tbl_size(_tbl)                                             \
//        ),                                                                  \
//        (void)0                                                             \
//    ))
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ assumes:
//      - all operands are of an unsigned integral type with equivalent bit sizes ...
//      - the size of the hash_tbl m, is a power of two, ie m === 2**M, M === log2(m),
//      - each hash_param is an odd number, not exceeding the largest prime for its word size,
//      - if giving a second hash_param b, then b < 2**(w - M), where w is the bit size of M
// vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// hash(x) = ((coef[0] * x + coef[1]) mod 2**w) /  2**(w-M)
// hash(x) = (coef[0] * x + coef[1]) /  2**(w-M)
// hash(x) = (coef[0] * x + coef[1]) >> (w - M),
// hash(x) = (coef[0] * x + coef[1]) >> (bit_size(m) - log2(m))
// hash(x) = (coef[0] * x + coef[1]) >> (bit_size(m) - leadn_one_index(m))
// 4 instructions (mult, subtration, shift, leadn_one_index) at a minimun of 4-5 instructions,
//      depending on the operands and compiler optimizations.
// see: @http://en.wikipedia.org/wiki/Universal_hashing#Avoiding_modular_arithmetic


#endif
