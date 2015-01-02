
#ifndef __COMP_ERROR__
#define __COMP_ERROR__

#include <assert.h>
#include <stddef.h>
#include <limits.h>

#ifndef _t
#   define _t typeof
#endif

#ifndef _s
#   define _s sizeof
#endif

#ifndef bit_sz
#   define bit_sz(type) (_s(type) * CHAR_BIT)
#endif

#ifndef bit_offst
#   define bit_offst(tp, memb) (offsetof(_t(tp), memb) * CHAR_BIT)
#endif


#ifndef intrsc_attrs
#ifdef __INTEL_COMPILER
#   define intrsc_attrs __attribute__((__gnu_inline__, __always_inline__, __artificial__)) //, __artificial__
#else
#   define intrsc_attrs __attribute__((__gnu_inline__, __always_inline__)) //, __artificial__
#endif
#endif

#ifndef static_inline
#   define static_inline static __inline__ intrsc_attrs
#endif


#define comp_time_assrt(expr) extern char __failed_assertion__[!!(expr) - 1]

#define comp_select         __builtin_choose_expr
//      ^^^^^^^^^^^ similar to ?: but requires the conditional to be an immdiate and the types aren't modified ...

#define comp_t_eq       __builtin_types_compatible_p
//      ^^^^^^^^^^^^^   compares two types returns 1 if they equal or if typedef, their underlying types match, otherwise 0
//                  compilation error if args are not types at all ....

#define comp_t_neq      !__builtin_types_compatible_p
//      ^^^^^^^^^^^^^^ inverts comp_t_eq

#define comp_expr_is_imm    __builtin_constant_p
//      ^^^^^^^^^^^^^^^^ returns 0 if expression is a compile time constant otherwise 0 ...
//              compilation error if argument is not an expression ...

#define comp_t_is_array(arg)  comp_t_eq(_t((arg)[0]) [], _t(arg))
//      ^^^^^^^^^^^^^ returns 1 if arg is an array of some sort, 0 otherwise (ie its a pointer, scalar, obj, ...)

#define comp_t_is_void_ptr(arg) comp_t_eq(_t(arg), void *)
//      ^^^^^^^^^^^^^^^^ returns 1 if arg is a void pointer, two pointers to diff types are considered diff ...


#ifndef array_cnt
#   define array_cnt(arr) (_s(arr)/_s((arr)[0]))
#endif


#endif
