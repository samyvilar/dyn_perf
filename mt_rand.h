#ifndef MT_RAND_H
#define MT_RAND_H

#include <stdio.h>
#include <string.h>
#include <limits.h>

#include "vect.h"

#ifndef _s
#   define _s sizeof
#endif

#ifndef _t
#   define _t typeof
#endif

#ifndef bit_sz
#   define bit_sz(a) (_s(a) * CHAR_BIT)
#endif

#define RAND_GEN_STATE_LENGTH 624U
#define mt_state_word_cnt(type) (RAND_GEN_STATE_LENGTH / (bit_sz(type) / bit_sz(int)))
//      ^^^^^^^^^^^^^^^^^ calc number words for the state table ...

#define mt_states(mt)               ((mt)->_states)
#define mt_state_entry(mt, index)   (mt_states(mt)[(index)])
#define mt_state_t(mt)              _t(mt_state_entry(mt, 0))

#define mt_states_cnt(mt)           mt_state_word_cnt(mt_state_t(mt))

#define mt_index(mt)                ((mt)->_index)
#define mt_set_index(mt, v)         (mt_index(mt) = (v))

#define mt_state_set_entry(mt, index, value) (mt_state_entry(mt, index) = (value))

#define mt_inc_index(mt)        mt_index(mt)++


typedef __attribute__ ((aligned (_s(vect.lrgst.intgl.type)))) struct mt_state_32_t {
    unsigned
        _states[mt_state_word_cnt(unsigned) + (_s(vect.lrgst.intgl.type)/_s(unsigned))];
        //                                ^^^^^^^^^^^^^^^^^^^^^^^ add extra buffer for vector operations ...
    unsigned _index;
} mt_state_32_t;


typedef __attribute__ ((aligned (_s(vect.lrgst.intgl.type)))) struct mt_state_64_t {
    unsigned long long
        _states[mt_state_word_cnt(unsigned long long) + (_s(vect.lrgst.intgl.type)/_s(unsigned long long))];
        //                                              ^^^^^^^^^^^ add extra buffer for vector operations ...
    unsigned _index;
} mt_state_64_t;


extern   mt_state_64_t  mt_vect_64;
unsigned long long      genrand64_int64();
unsigned long long      mt_rand_vect_64();

extern   mt_state_32_t  mt_vect_32;
unsigned int            genrand_int32();
unsigned int            mt_rand_vect_32();


#endif // MT_RAND_H
