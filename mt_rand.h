#ifndef MT_RAND_H
#define MT_RAND_H

#include "comp_utils.h"
#include "vect.h"


#define RAND_GEN_STATE_LENGTH 624U
#define mt_state_word_cnt(type) (RAND_GEN_STATE_LENGTH / (_s(type) / _s(int)))
//      ^^^^^^^^^^^^^^^^^ calc number words for the state table ...

#define mt_states(mt)               ((mt)->_states)
#define mt_state_entry(mt, index)   (mt_states(mt)[(index)])
#define mt_state_t(mt)              _t(mt_state_entry(mt, 0))

#define mt_states_cnt(mt)           mt_state_word_cnt(mt_state_t(mt))

#define mt_index(mt)                ((mt)->_index)
#define mt_set_index(mt, v)         (mt_index(mt) = (v))


#define mt_inc_index(mt)             mt_index(mt)++


typedef struct mt_state_32_t {
    unsigned
        _states[mt_state_word_cnt(unsigned) + (_s(vect_lrgst_intgl_type)/_s(unsigned))];
        //                                ^^^^^^^^^^^^^^^^^^^^^^^ add extra buffer for vector operations ...
    size_t _index;
} __attribute__ ((aligned (_s(vect_lrgst_intgl_type)))) mt_state_32_t;


typedef struct mt_state_64_t {
    unsigned long long
        _states[mt_state_word_cnt(unsigned long long) + (_s(vect_lrgst_intgl_type)/_s(unsigned long long))];
        //                                              ^^^^^^^^^^^ add extra buffer for vector operations ...
    size_t _index;
} __attribute__ ((aligned (_s(vect_lrgst_intgl_type)))) mt_state_64_t;


extern   mt_state_64_t  mt_vect_64;
unsigned long long      genrand64_int64();
unsigned long long      mt_rand_vect_64();

double mt_rand_vect_double_0_1();

vect_lrgst_intgl_type
    mt_rand_packd_temprd_bits_64();


extern   mt_state_32_t  mt_vect_32;
unsigned int            genrand_int32();
unsigned int            mt_rand_vect_32();

vect_lrgst_intgl_type
    mt_rand_packd_temprd_bits_32();


#endif // MT_RAND_H
