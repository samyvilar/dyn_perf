
#ifndef __COMP_ERROR__
#define __COMP_ERROR__

#include <assert.h>

#define assert_with_msg(expr, msg) assert((msg && (expr)))
#define comp_error_init(msg) ({ assert_with_msg(0, msg); (void)0;})
#define comp_error_proc(expr) ({ typeof(expr) __ = (expr); })

#ifndef _t
#   define _t typeof
#endif

#ifndef _s
#   define _s sizeof
#endif

#ifndef bit_size
#   define bit_size(type) (_s(type) * (_s(char) * CHAR_BIT))
#endif


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

#define comp_t_is_array(arg)  comp_t_eq(_t(arg[0]) [], _t(arg))
//      ^^^^^^^^^^^^^ returns 1 if arg is an array of some sort, 0 otherwise (ie its a pointer, scalar, obj, ...)

#define comp_t_is_void_ptr(arg) comp_t_eq(_t(arg), void *)
//      ^^^^^^^^^^^^^^^^ returns 1 if arg is a void pointer, two pointers to diff types are considered diff ...


#define comp_unlikely(expr) __builtin_expect(expr, 0)
#define comp_likely(expr)   __builtin_expect(expr, 1)


#endif
