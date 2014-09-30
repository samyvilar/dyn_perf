
#ifndef __MACRO_UTILS__
#define __MACRO_UTILS__

#include <stdlib.h>

//#include "words.h"

#define _remove_x(x)
#define conv_to_macro_func(x) _remove_x x
#define cons_paren(x) conv_to_macro_func x // convs ((expr_a)expr_b) to expr_b by creating conv_to_macro_func((expr_a)expr_b) -> _remove_x(expr_a)expr_b -> expr_b

#if cons_paren(NULL) // if NULL is non-zero use comparison operators
    #define is_not_null(v)  (NULL != (v))
    #define is_null(v)      (NULL == (v))
#else   // NULL must be zero so no need for comparison ..
    #define is_not_null(v)  (v)
    #define is_null(v)      (!is_not_null(v))
#endif



#define macro_repeat_0(...)
#define macro_repeat_1(...) __VA_ARGS__
#define macro_repeat_2(...) macro_repeat_1(__VA_ARGS__) macro_repeat_1(__VA_ARGS__)
#define macro_repeat_4(...) macro_repeat_2(__VA_ARGS__) macro_repeat_2(__VA_ARGS__)
#define macro_repeat_7(...) macro_repeat_4(__VA_ARGS__) macro_repeat_2(__VA_ARGS__) macro_repeat_1(__VA_ARGS__)
#define macro_repeat_8(...) macro_repeat_4(__VA_ARGS__) macro_repeat_4(__VA_ARGS__)
#define macro_repeat_(cnt)  macro_repeat_ ## cnt

#define macro_apply(f, ...) f (__VA_ARGS__)
// apply macro function to already parentheisized list of arguments ...


#define macro_arg_0(arg_0, args...) arg_0
#define macro_arg_1(arg_0, arg_1, ...) arg_1
#define macro_arg_2(arg_0, arg_1, arg_2, ...) arg_2
#define macro_arg_3(arg_0, arg_1, arg_2, arg_3, ...) arg_3
#define macro_arg_4(arg_0, arg_1, arg_2, arg_3, arg_4, ...) arg_4
#define macro_arg_5(arg_0, arg_1, arg_2, arg_3, arg_4, arg_5, ...) arg_5

#define macro_arg(_imm_literal, args...) macro_arg_ ## _imm_literal (args)


#define macro_select_args_2(arg_0, arg_1, ...) arg_0, arg_1
#define macro_select_args_4(arg_0, arg_1, ...) arg_0, arg_1, macro_select_args_2(__VA_ARGS__)
#define macro_select_args_8(arg_0, arg_1, arg_2, arg_3, ...) arg_0, arg_1, arg_2, arg_3, macro_select_args_4(__VA_ARGS__)
#define macro_select_args_16(arg_0, arg_1, arg_2, arg_3, arg_4, arg_5, arg_6, arg_7, ...)\
    arg_0, arg_1, arg_2, arg_3, arg_4, arg_5, arg_6, arg_7, macro_select_args_8(__VA_ARGS__)

#define macro_select_args(cnt, ...) macro_select_args_ ## cnt (__VA_ARGS__)


//#define next(obj) ((obj)->_next)
//#define set_next(obj, v) (next(obj) = (v))


#define macro_unroll(macro_or_func, __times, args...) ({    \
    register unsigned long __block_cnt = (__times) / 4;     \
    __block_cnt = __block_cnt ? __block_cnt : 1;            \
    switch ((__times) % 4) {                                \
        case 0: do {                                        \
                macro_or_func(args);                        \
        case 3: macro_or_func(args);                        \
        case 2: macro_or_func(args);                        \
        case 1: macro_or_func(args);                        \
        } while (--__block_cnt);                            \
    }                                                       \
}) // ^^^^^^^^^^^ unrolls an operation or loop, reducing cost of branch predictions ... see: duffs device.
#endif
