

#ifndef __BITS_H__
#define __BITS_H__

#include "comp_utils.h"
#include "scalrs.h"

//#define bits_set_trlng_ones(cnt) (((_t(cnt))1 << (cnt)) - (_t(ones_cnt))1)
////      ^^^^ return a seq of ones, using the fact that 0b100000 - 1 == 0b011111,
////           that has the same type as bit_cnt


//#define bits_set_trlng(cnt) ({typedef _t(cnt) _word_t; (((_word_t)1 << (cnt)) - (_word_t)1); })

#define instr_postfix_0 ""  // no operands.
#define instr_postfix_1 "b" // byte (8 bits) operand(s)
#define instr_postfix_2 "w" // short (16 bits) operand(s)
#define instr_postfix_4 "l" // int (32 bits) operand(s)
#define instr_postfix_8 "q" // long (64 bits) operand(s)
#define get_instr_postfix(size) instr_postfix_ ## size
#define att_instr(instr_name, operand_size) #instr_name get_instr_postfix(operand_size)
//      ^^^^^^^^^ giving the instr_name as an identifier
//                and the expected operand_size 1 of (1, 2, 4, 8), (use 0 for no operands)
//                @returns the instruction as a literal string.
#ifdef __INTEL_COMPILER // <<<<<< icc seems to be missing some of the builtin intrinsics that are part of gcc/clang
    static_inline unsigned short intrsc_attrs __builtin_clzs(unsigned short x) {
    //  ^^^^^^^^^^^^^^ count leading zeros on a short (16 bit intgl), return 15 if all zeros ...
        asm (att_instr(bsr, 2) " %0, %0\t\nxor $15, %0\t\n" : "=r" (x) : "0"(x));
        return x;
    }

    static_inline unsigned short intrsc_attrs __builtin_ctzs(unsigned short x){
    //  ^^^^^^^^^^^^^^ counts trailing zeros on a short (16 bit intgl), return 0 if 0 ...
        asm (att_instr(bsf, 2) " %0, %0" : "=r" (x) : "0"(x));
        return x;
    }

#   define __builtin_clzb(x)   __builtin_clz((unsigned int)(x) | 0xFFFFFF00)
    //      ^^^^^^^^^^^^^^ count leading zeros of a byte,
    //                     __builtin_clz uses unsigned int, so char -> unsigned int adds 24 zeros.
#   define __builtin_ctzb(x)   __builtin_ctz((unsigned int)(x) | 0xFFFFFF00)
    //      ^^^^^^^^^^^^^^ count trailing zeros of a byte
#else
#   define __builtin_clzb(x)    __builtin_clzs((unsigned short)(x) | (unsigned short)0xFF00)
#   define __builtin_ctzb(x)    __builtin_ctzs((unsigned short)(x) | (unsigned short)0xFF00)
/************************************************************************************************/
#endif

static_inline unsigned short intrsc_attrs __builtin_popcounts(unsigned short x) {
    asm (att_instr(popcnt, 2) " %0, %0" : "=r" (x) : "0"(x));
    return x;
}

#define __builtin_popcountb  __builtin_popcounts


#define bits_leadn_zrs_8  __builtin_clzb
#define bits_leadn_zrs_16 __builtin_clzs
#define bits_leadn_zrs_32 __builtin_clz
#define bits_leadn_zrs_64 __builtin_clzll
/* ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
 * Counts the number of leading zeros, generates two instructions
 * bsr    %ecx, %ecx, bit scan reverse.
 * xor    $31, %ecx
**/

#define bits_trlng_zrs_8  __builtin_ctzb
#define bits_trlng_zrs_16 __builtin_ctzs
#define bits_trlng_zrs_32 __builtin_ctz
#define bits_trlng_zrs_64 __builtin_ctzll
/*^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
 * Counts the number of trailing zeros,
 * generates a single instruction
 *      bsf, bit scan forward,
 *      ~3 cycles on most modern intel x86 chips
**/

#define bits_cnt_ones_8  __builtin_popcountb
#define bits_cnt_ones_16 __builtin_popcounts
#define bits_cnt_ones_32 __builtin_popcount
#define bits_cnt_ones_64 __builtin_popcountll
/*^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
 * Counts the number of set ones, will either generate several
 * instructions or a single popcntl if compiled with sse4.2 (intel) or sse4a (AMD)
**/

#define bits_trlng_one_8  __builtin_ffs
#define bits_trlng_one_16 __builtin_ffs
#define bits_trlng_one_32 __builtin_ffs
#define bits_trlng_one_64 __builtin_ffsll


#define singl_param_integral_macro(func_name, param, ret_type) \
    scalr_switch_on_byte_size(  \
        param,                  \
        func_name ## 64(param), \
        func_name ## 32(param), \
        func_name ## 16(param), \
        func_name ## 8(param),  \
        (void)0                 \
    )

#define bits_leadn_zrs(x) singl_param_integral_macro(bits_leadn_zrs_, x, unsigned)
#define bits_trlng_zrs(x) singl_param_integral_macro(bits_trlng_zrs_, x, unsigned)
#define bits_trln_one(x)  singl_param_integral_macro(bits_trlng_one_, x, unsigned)
#define bits_cnt_ones(x)  singl_param_integral_macro(bits_cnt_ones_, x, unsigned)

#define _leadn_one_index_asm(x, imm_byt_size) ({    \
    _t(x) dest = (x);                               \
    asm (att_instr(bsr, imm_byt_size) " %0, %0" : "=r" (dest) : "0"(dest));\
    dest; })

#define bits_leadn_one(x)            \
    scalr_switch_on_byte_size(       \
        x,                           \
        _leadn_one_index_asm(x, 8),  \
        _leadn_one_index_asm(x, 4),  \
        _leadn_one_index_asm(x, 2),  \
        _leadn_one_index_asm(x, 1),  \
        (void)0                      \
    )
/*^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
 * Returns the position of the leading one,
 *  generates a single instruction bsr
**/


#endif