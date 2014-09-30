
#ifndef __SCALR_TYPES__
#define __SCALR_TYPES__

#include <stdint.h>
#include <limits.h>

#include "comp_utils.h"


#if   (UINTPTR_MAX == UINT64_MAX)   // convert void * to integral type so we can apply xor and other operators..
    typedef long long int       word_t;
    typedef unsigned long long  uword_t;
#elif (UINTPTR_MAX == UINT32_MAX)
    typedef int                 word_t;
    typedef unsigned            uword_t;
#elif (UINTPTR_MAX == UINT16_MAX)
    typedef short               word_t;
    typedef unsigned short      uword_t;
#elif (UINTPTR_MAX == UINT8_MAX)
    typedef char                word_t;
    typedef unsigned char       uword_t;
#else
    #error "Unable to determing intergral type for pointer type"
#endif

#define cast_ptr(var) ((uword_t)(var)) // cast pointer to integral type ...


#define t_bit_name(_kind, _bit_size) _kind ## _bit_size ## bit

#define sint1byt_t    signed      char
#define uint1byt_t    unsigned    char
#define sint8bit_t    signed      char
#define uint8bit_t    unsigned    char

#define sint2byt_t    signed      short
#define uint2byt_t    unsigned    short
#define sint16bit_t   signed      short
#define uint16bit_t   unsigned    short

#define sint4byt_t    signed      int
#define uint4byt_t    unsigned    int
#define sint32bit_t   signed      int
#define uint32bit_t   unsigned    int

#define sint8byt_t    signed      long long int
#define uint8byt_t    unsigned    long long int

#define sint64bit_t   signed      long long int
#define uint64bit_t   unsigned    long long int

#define SCALR_INTGL_TYPE_CNT 8

#define sint_byt_t(_byt_mag) sint ## _byt_mag ## byt ## _t    // get signed integral type by byte size
#define uint_byt_t(_byt_mag) uint ## _byt_mag ## byt ## _t   // get unsigned integral type with with _byt_mag bytes ..

#define sint_bit_t(_bit_mag) sint ## _bit_mag ## bit ## _t   // get signed integral type by bit size
#define uint_bit_t(_bit_mag) uint ## _bit_mag ## bit ## _t   // get unsigned integral by bit size

#define flt4byt_t    float
#define flt32bit_t   float
#define flt8byt_t    double
#define flt64bit_t   double

#define flt_byt_t(_byt_mag) flt ## _byt_mag ## byt ## _t
#define flt_bit_t(_bit_mag) flt ## _bit_mag ## bit ## _t

#define t_from_kind_bit_size(_kind, _bit_size) _kind ## _bit_t(_bit_size)


#define t_is_flt32bit(type) (comp_t_eq(type, flt_bit_t(32)))
#define t_is_flt64bit(type) (comp_t_eq(type, flt_bit_t(64)))
#define t_is_flt(type)      (t_is_flt32bit(type) || t_is_flt64bit(type))
#define expr_is_flt(expr)   t_is_flt(_t(expr))

#define t_is_uint8bit(type)   comp_t_eq(type, uint_bit_t(8))
#define t_is_uint16bit(type)  comp_t_eq(type, uint_bit_t(16))
#define t_is_uint32bit(type)  comp_t_eq(type, uint_bit_t(32))

#define t_is_uint64bit(type)  (comp_t_eq(type, uint_bit_t(64)) || comp_t_eq(type, unsigned long int))

#define expr_is_(_expr, _kind, _bit_size) t_is_ ## _kind ## _bit_size ## bit(_t(_expr))
#define t_is_   expr_is_

#define t_is_sint8bit(type)   (comp_t_eq(type, sint_bit_t(8)) || comp_t_eq(type, char))

// very strange ==> __builtin_types_compatible_p(signed char, char) == 0,
// which seems to stem from 3.9.1 Fundamental types [basic.fundamental],
// http://stackoverflow.com/questions/436513/char-signed-char-char-unsigned-char

#define t_is_sint16bit(type)  comp_t_eq(type, sint_bit_t(16))
#define t_is_sint32bit(type)  comp_t_eq(type, sint_bit_t(32))
#define t_is_sint64bit(type)  (comp_t_eq(type, sint_bit_t(64)) || comp_t_eq(type, signed long int))

#define _t_is_int_t(type, int_t_kind)  (  \
    comp_t_eq(type, int_t_kind(8))           \
 || comp_t_eq(type, int_t_kind(16))          \
 || comp_t_eq(type, int_t_kind(32))          \
 || comp_t_eq(type, int_t_kind(64))          \
)

#define t_is_uint(type) _t_is_int_t(type, uint_bit_t)
#define t_is_sint(type) (_t_is_int_t(type, sint_bit_t) || t_is_sint8bit(type))

#define scalr_t_eq(type_a, type_b) (                            \
       comp_t_eq(type_a, type_b)                                \
    || (t_is_sint64bit(type_a) && t_is_sint64bit(type_b))       \
    || (t_is_uint64bit(type_a) && t_is_uint64bit(type_b))       \
    || (t_is_sint8bit(type_a)  && t_is_sint8bit(type_b))        \
)// no choice since comp_types_eq doesn't properly handle (signed char, char) and (signed long int, signed long long int)
 // eventhough on most compilers for x86_64 system their pretty much identical ...

#define scalr_t_neq(type_a, type_b) (!(scalr_t_eq(type_a, type_b)))



#define expr_is_uint(expr)  t_is_uint(_t(expr))
#define expr_is_sint(expr)  t_is_sint(_t(expr))

#define t_is_intgl(type) (t_is_uint(type) || t_is_sint(type))
//      ^^^^^^^^^^^^^ returns 1 if type is a native integral type, 0 otherwise ....
#define expr_is_intgl(expr) t_is_intgl(_t(expr))
//      ^^^^^^^^^^^^^ returns 1 if expr returns a native integral type, 0 otherwise ....

#define expr_is_intgl_bit(expr, bit_size) (expr_is_(expr, sint, bit_size) || expr_is_(expr, uint, bit_size))

#define t_is_scalr(_type) (t_is_intgl(_type) || t_is_flt(_type))
#define expr_is_scalr(expr) t_is_scalr(_t(expr))
//      ^^^^^^^^^^^^^ returns 1 if expr returns a native scalar type, 0 otherwise


#define _interp(e, _from_name, _to_name) (((union {_from_name _from; _to_name _to;}){(e)})._to)
// arithmetic right shift (extending sign bit)


// apply bit operations on scalar expression, if floats reinterpret words as intgl scalars apply bit operation
// and reinterpret result back as float type ...
#define scalr_bit_oper(a, oper, b, _flt_to_intl_kind)       \
    macro_apply(                                            \
        scalr_switch,                                       \
        (a),                                                \
        _interp(                                            \
            (_interp(a, flt_bit_t(64), _flt_to_intl_kind(64)) oper _interp(b, flt_bit_t(64), _flt_to_intl_kind(64))),     \
            _flt_to_intl_kind(64),                          \
            flt_bit_t(64)                                   \
        ),                                                  \
        _interp(                                            \
            (_interp(a, flt_bit_t(32), _flt_to_intl_kind(32)) oper _interp(b, flt_bit_t(32), _flt_to_intl_kind(32))),     \
            _flt_to_intl_kind(32),                          \
            flt_bit_t(32)                                   \
        ),                                                  \
                                                            \
        ((uint_bit_t(64))(a)    oper (uint_bit_t(64))(b)),  \
        ((uint_bit_t(32))(a)    oper (uint_bit_t(32))(b)),  \
        ((uint_bit_t(16))(a)    oper (uint_bit_t(16))(b)),  \
        ((uint_bit_t(8))(a)     oper (uint_bit_t(8))(b)),   \
                                                            \
        ((sint_bit_t(64))(a)    oper (sint_bit_t(64))(b)),  \
        ((sint_bit_t(32))(a)    oper (sint_bit_t(32))(b)),  \
        ((sint_bit_t(16))(a)    oper (sint_bit_t(16))(b)),  \
        ((sint_bit_t(8))(a)     oper (sint_bit_t(8))(b)),   \
        (void)0                                             \
    )

#define scalr_xor(a, b) scalr_bit_oper(a, ^, b, uint_bit_t)
#define scalr_and(a, b) scalr_bit_oper(a, &, b, uint_bit_t)
#define scalr_or(a, b)  scalr_bit_oper(a, |, b, uint_bit_t)

#define scalr_cmp_bits_eq(a, b)     scalr_bit_oper(a, ==, b, uint_bit_t)
#define scalr_cmp_bits_neq(a, b)    scalr_bit_oper(a, !=, b, uint_bit_t)

#define sclar_shift(shf_sclr_a, shift_oper, _mb, _intgl_kind)           \
    macro_apply(                                                        \
        scalr_switch,                                                   \
        shf_sclr_a,                                                     \
        _interp(                                                        \
            (_interp(shf_sclr_a, flt_bit_t(64), _intgl_kind(64)) shift_oper (_mb)),   \
            _intgl_kind(64),                                            \
            flt_bit_t(64)                                               \
        ),                                                              \
        _interp(                                                        \
            (_interp(shf_sclr_a, flt_bit_t(32), _intgl_kind(32)) shift_oper (_mb)),   \
            _intgl_kind(32),                                            \
            flt_bit_t(32)                                               \
        ),                                                              \
        ((_intgl_kind(64))(shf_sclr_a)    shift_oper (_mb)),            \
        ((_intgl_kind(32))(shf_sclr_a)    shift_oper (_mb)),            \
        ((_intgl_kind(16))(shf_sclr_a)    shift_oper (_mb)),            \
        ((_intgl_kind(8))(shf_sclr_a)     shift_oper (_mb)),            \
                                                                        \
        ((_intgl_kind(64))(shf_sclr_a)    shift_oper (_mb)),            \
        ((_intgl_kind(32))(shf_sclr_a)    shift_oper (_mb)),            \
        ((_intgl_kind(16))(shf_sclr_a)    shift_oper (_mb)),            \
        ((_intgl_kind(8))(shf_sclr_a)     shift_oper (_mb)),            \
        (void)0                                     \
    )


#define scalr_lshift(a, b)  sclar_shift(a, <<, b, uint_bit_t)
#define scalr_lshift_imm    scalr_lshift
#define scalr_lshift_scalr  scalr_lshift

#define scalr_rshift_logic(a, b)  sclar_shift(a, >>, b, uint_bit_t)
#define scalr_rshift_logic_imm    scalr_rshift_logic
#define scalr_rshift_logic_scalr  scalr_rshift_logic

#define scalr_rshift_arith(a, b)  sclar_shift(a, >>, b, sint_bit_t)
#define scalr_rshift_arith_imm    scalr_rshift_arith
#define scalr_rshift_arith_scalr  scalr_rshift_arith

#define scalr_add(a, b) ((a) + (b))
#define scalr_sub(a, b) ((a) - (b))
#define scalr_mul(a, b) ((a) * (b))

#define scalr_log2 cnt_leadn_zrs
#define scalr_mul_by_pow2 scalr_lshift
#define scalr_div_by_pow2 scalr_rshift

#define scalr_div(a, b) ((a) / (b))


#define scalr_raise_pow_2(value) ((_t(value))1 << (value))

#define scalr_sign_ext(a)                                       \
    scalr_switch(                                               \
        a,                                                      \
        _interp((_interp(a, flt_bit_t(64), sint_bit_t(64)) >> 63),  sint_bit_t(64), flt_bit_t(64)),    \
        _interp((_interp(a, flt_bit_t(32), sint_bit_t(32)) >> 31),  sint_bit_t(32), flt_bit_t(32)),    \
        (sint_bit_t(64))((a) >> 63),                            \
        (sint_bit_t(32))((a) >> 31),                            \
        (sint_bit_t(16))((a) >> 15),                            \
        (sint_bit_t(8)) ((a) >> 7),                             \
        (uint_bit_t(64))((sint_bit_t(64))(a) >> 63),            \
        (uint_bit_t(32))((sint_bit_t(32))(a) >> 31),            \
        (uint_bit_t(16))((sint_bit_t(16))(a) >> 15),            \
        (uint_bit_t(8)) ((sint_bit_t(8))(a)  >> 7),             \
        (void)0                                                 \
    )


/** SELECT based on type of expression ****/
#define scalr_switch_sint(expr, if_sint64bit, if_sint32bit, if_sint16bit, if_sint8bit, if_no_match)   \
    (comp_select(expr_is_(expr, sint, 64), if_sint64bit,  \
     comp_select(expr_is_(expr, sint, 32), if_sint32bit,  \
     comp_select(expr_is_(expr, sint, 16), if_sint16bit,  \
     comp_select(expr_is_(expr, sint, 8),  if_sint8bit,   \
        if_no_match)))))

#define scalr_switch_uint(expr, if_uint64bit, if_uint32bit, if_uint16bit, if_uint8bit, if_no_match)   \
    (comp_select(expr_is_(expr, uint, 64), if_uint64bit, \
    comp_select(expr_is_(expr, uint, 32), if_uint32bit,  \
    comp_select(expr_is_(expr, uint, 16), if_uint16bit,  \
    comp_select(expr_is_(expr, uint, 8),  if_uint8bit,   \
        if_no_match)))))


#define scalr_switch_oblvs_sign_intgl(expr, if_intgl64bit, if_intgl32bit, if_intgl16bit, if_intgl8bit, if_no_match)  \
    comp_select(expr_is_intgl_bit(expr, 64), if_intgl64bit, \
    comp_select(expr_is_intgl_bit(expr, 32), if_intgl32bit, \
    comp_select(expr_is_intgl_bit(expr, 16), if_intgl16bit, \
    comp_select(expr_is_intgl_bit(expr, 8),  if_intgl8bit,  \
        if_no_match))))


// select expression on floats
#define scalr_switch_flt(expr, if_flt64bit, if_flt32bit, if_no_match) \
    comp_select(expr_is_(expr, flt, 64), if_flt64bit,       \
    comp_select(expr_is_(expr, flt, 32), if_flt32bit,       \
        if_no_match))

// select expression on integrals ...
#define scalr_switch_intgl(         \
    expr,                           \
                                    \
    if_sint64bit,                   \
    if_sint32bit,                   \
    if_sint16bit,                   \
    if_sint8bit,                    \
                                    \
    if_uint64bit,                   \
    if_uint32bit,                   \
    if_uint16bit,                   \
    if_uint8bit,                    \
                                    \
    if_no_match                     \
)                                   \
    scalr_switch_sint(              \
        expr,                       \
        if_sint64bit,               \
        if_sint32bit,               \
        if_sint16bit,               \
        if_sint8bit,                \
        scalr_switch_uint(          \
            expr,                   \
            if_uint64bit,           \
            if_uint32bit,           \
            if_uint16bit,           \
            if_uint8bit,            \
                                    \
            if_no_match             \
        )                           \
    )


// @@>> DO NOT CHANGE THIS ORDER!!!!
//#define flts_sizes     (flt, 64), (flt, 32)
//#define sints_sizes    (sint, 64), (sint, 32), (sint, 16), (sint, 8)
//#define uints_sizes    (uint, 64), (uint, 32), (uint, 16), (uint, 8)
//#define scalr_intlg_cnt 8

//#define intgls_sizes   sints_sizes, uints_sizes
//#define scalrs_params_kinds_sizes   flts_sizes, intgls_sizes
/**************************************************************************/

//#define type_bit_name_list(_kind_and_size) t_bit_name _kind_and_size
//#define scalrs_names MAP_LIST(type_bit_name_list, scalrs_params_kinds_sizes)

// select expression on scalars
#define scalr_switch(               \
    expr,                           \
                                    \
    if_flt64bit,                    \
    if_flt32bit,                    \
                                    \
    if_sint64bit,                   \
    if_sint32bit,                   \
    if_sint16bit,                   \
    if_sint8bit,                    \
                                    \
    if_uint64bit,                   \
    if_uint32bit,                   \
    if_uint16bit,                   \
    if_uint8bit,                    \
                                    \
    if_no_match                     \
) ({                                \
    typedef _t(expr) _expr_scalr_switch_t; \
    scalr_switch_flt(               \
        _expr_scalr_switch_t,       \
        if_flt64bit,                \
        if_flt32bit,                \
        scalr_switch_sint(          \
            _expr_scalr_switch_t,   \
            if_sint64bit,           \
            if_sint32bit,           \
            if_sint16bit,           \
            if_sint8bit,            \
            scalr_switch_uint(      \
                _expr_scalr_switch_t,\
                if_uint64bit,       \
                if_uint32bit,       \
                if_uint16bit,       \
                if_uint8bit,        \
                                    \
                if_no_match         \
            )                       \
        )                           \
    ); })

#define scalr_switch_on_byte_size(expr, if_8bytes, if_4bytes, if_2bytes, if_1bytes, if_no_match) \
    comp_select(_s(expr) == 8, if_8bytes,   \
    comp_select(_s(expr) == 4, if_4bytes,   \
    comp_select(_s(expr) == 2, if_2bytes,   \
    comp_select(_s(expr) == 1, if_1bytes,   \
        if_no_match))))

#define scalr_switch_flt_or_intgl_byte_sizes(expr, if_flt8bytes, if_flt4bytes, if_8bytes, if_4bytes, if_2bytes, if_1bytes, if_no_match) ({\
    typedef _t(expr) _exprscalr_intf_byt_size_t;                                \
    comp_select(expr_is_(_exprscalr_intf_byt_size_t, flt, 64), if_flt8bytes,    \
    comp_select(expr_is_(_exprscalr_intf_byt_size_t, flt, 32), if_flt4bytes,    \
        scalr_switch_on_byte_size(_exprscalr_intf_byt_size_t, if_8bytes, if_4bytes, if_2bytes, if_1bytes, if_no_match))); })


#include <stdio.h>
#define scalr_str(buffer, _expr) ({             \
    _t(buffer[0]) *_temp = (buffer);            \
    snprintf(                                   \
        _temp,                                  \
        _s(buffer),                             \
        scalr_switch(                           \
            _expr,                              \
            "%f", "%f",                         \
            "%lli", "%i", "%hi", "%hhi",        \
            "%llu", "%u", "%hu", "%hhu",        \
            (void)0                             \
        ),                                      \
        (_t(scalr_switch(_expr,                 \
            (double)0, (double)0,               \
            (signed long long int)0, (int)0, (short)0, (char)0,     \
            (unsigned long long int)0, (unsigned)0, (unsigned short)0, (unsigned char)0,   \
            (void)0)))(_expr)                   \
    ); _temp; })


#define _xor_symbl  ^
#define _or_symbl   |
#define _and_symbl  &
#define _add_symbl  +
#define _sub_symbl  -
#define _mul_symbl  *
#define _lshift_symbl <<
#define _rshift_symbl >>

#define _lshift_imm_symbl   _lshift_symbl
#define _lshift_scalr       _lshift_symbl

#define _rshift_arith_symbl _rshift_symbl
#define _rshift_logic_symbl _rshift_symbl

#define oper_symbl(name) _ ## name ## _symbl
#define token_str(token) #token

#define scalr_oper(oper) scalr_ ## oper

#define scalr_is_zero(value)     ((value) == (_t(value))0)
#define scalr_is_not_zero(value) ((value) != (_t(value))0)

#define scalr_square(value)         ((value) * (value))
#define scalr_doubl(value)          (2 * (value))
#define scalr_half(value)           ((value) / 2)
#define scalr_quartr(value)         ((value) / 4)

#define scalr_is_not_pow_2(value)  (((value) & ((value) - 1)) != 0)
#define scalr_is_pow_2(value)      (((value) & ((value) - 1)) == 0)


#define max_by_cmp(a, b)        (((a) < (b)) ? (b) : (a))
#define max_by_subt(a, b)		(a + ((b - a) & scalr_sign_ext(a - b)))
#define max_by_xor(a, b)        (a ^ ((a ^ b) & scalr_sign_ext(a - b)))
#define max_by_subt_cpy(a, b)   ((_t(a)[2]){(a), (b)}[sign_bit_bool((a) - (b))])

#define scalr_max_by  max_by_xor

static inline void *max_void_p(register const uword_t a, register const uword_t b) {
    return (void *)scalr_max_by(a, b);
//    return (void *)(a ^ ((a ^ b) & ((word_t)(a - b) >> ((sizeof(word_t) * (sizeof(char) * 8)) - 1))));
}

#define scalr_max(a, b) ({              \
    register const _t(b) expr_b = (b);  \
    register const _t(a) expr_a = (a);  \
    (comp_t_is_void_ptr(expr_a) || comp_t_is_void_ptr(expr_b))  \
        ? max_void_p(cast_ptr(expr_a), cast_ptr(expr_b))        \
        : scalr_max_by(expr_a, expr_b);                         \
})

//#define max max_by_subt



//// min-max functions ... ********************************************************************
//#define min_by_cmp(a, b)        (((a) < (b)) ? (a) : (b))
//#define min_by_subt(a, b)    	((b) + (((a) - (b)) & sign_ext((a) - (b))))
//#define min_by_xor(a, b)    	((b) ^ (((a) ^ (b)) & sign_bit_ext((a) - (b))))
//
//// a bit safer since the exprs are temporary copied and the exprs only appear twice withing the macro
//#define min_by_subt_cpy(a, b)    ((typeof(a)[2]){(b), (a)}[sign_bit_bool((a) - (b))])
//
//#define min_p(a, b) ((void *)min(cast_ptr(a), cast_ptr(b)))
//
//#define min min_by_subt


// the returned valued of two scalar expression a and b equal if a - b == 0 or  a xor b == 0,
// otherwise a xor b != 0, it must have at least 1 set bit





#endif
