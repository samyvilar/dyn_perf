#ifndef MT_RAND_H
#define MT_RAND_H

#include <stdio.h>
#include <string.h>
#include <limits.h>

#ifndef bit_size
#   define bit_size(tpe_or_expr) (sizeof(tpe_or_expr) * CHAR_BIT)
#endif

#define RAND_GEN_STATE_LENGTH 624U
#define mt_state_word_cnt(type) (RAND_GEN_STATE_LENGTH / (bit_size(type) / bit_size(int)))
//      ^^^^^^^^^^^^^^^^^ calc number words for the state table ...

#define mt_states(mt)               ((mt)->_states)
#define mt_state_entry(mt, index)   (mt_states(mt)[(index)])
#define mt_state_t(mt)              typeof(mt_state_entry(mt, 0))

#define mt_states_cnt(mt)           mt_state_word_cnt(mt_state_t(mt))

#define mt_index(mt)        ((mt)->_index)
#define mt_set_index(mt, v) (mt_index(mt) = (v))

#define mt_state_set_entry(mt, index, value) (mt_state_entry(mt, index) = (value))

#define mt_inc_index(mt)        mt_index(mt)++


typedef struct mt_state_32_t {
    unsigned
        _states[mt_state_word_cnt(unsigned) + 16];
    /* add extra buffer for vector operations ...*/
    unsigned _index;
} mt_state_32_t;


typedef __attribute__ ((aligned (16))) struct mt_state_64_t {
    unsigned long long
        _states[mt_state_word_cnt(unsigned long long) + 16];
    /* add extra buffer for vector operations ...*/
    unsigned _index;
} mt_state_64_t;


extern mt_state_32_t mt_vect_32;
extern mt_state_64_t mt_vect_64;

extern unsigned mt_32[];
extern unsigned long long mt_64[];

unsigned long long genrand64_int64();
unsigned genrand_int32();

unsigned long long mt_rand_vect_64();
unsigned mt_rand_vect_32();


#endif // MT_RAND_H
