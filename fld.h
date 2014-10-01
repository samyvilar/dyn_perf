

#ifndef __FLD_H__
#define __FLD_H__

#include <stdlib.h>

#include "alloc.h"
#include "bits.h"
#include "comp_utils.h"


#define bits_pow_2(expn) (((_t(expn))1) << (expn))

#define fld_wrd_set(word, index) ((word) |=  (_t(word))bits_pow_2(index))
#define fld_wrd_clr(word, index) ((word) &= ~(_t(word))bits_pow_2(index))
#define fld_wrd_bit(word, index) ((word) &   (_t(word))bits_pow_2(index))

typedef struct fld_t {
    unsigned short *words;
    struct fld_t *_next;
} fld_t;

alloc_recl_sign_templs(fld);

fld_t *cleand_flds[bit_size(size_t)];

#define fld_calc_length(fld, capct) ((capct) / bit_size((fld)->words[0]))

static inline fld_t *fld_pow2_new(size_t capct) {
    register fld_t **bucket = &cleand_flds[bits_leadn_one(capct)], *self;

    if (*bucket) {
        self = *bucket;
        *bucket = (*bucket)->_next;
        self->words[0] = 0;
        return self;
    }

    self = fld_alloc();
    self->words = calloc(fld_calc_length(self, capct), _s(self->words[0]));
    return self;
}

static inline void fld_pow2_recl_clnd(fld_t *fld, size_t capct) {
    register fld_t **bucket = &cleand_flds[bits_leadn_one(capct)];
    fld->_next = *bucket;
    *bucket = fld;
}

#define fld_wrd(fld, index) ((fld)->words[(index)/bit_size((fld)->words[0])])

#define fld_bit_index(fld, index) ((index) % bit_size((fld)->words[0]))

#define fld_get(fld, index) fld_wrd_bit(fld_wrd(fld, index), fld_bit_index(fld, index))
#define fld_set(fld, index) fld_wrd_set(fld_wrd(fld, index), fld_bit_index(fld, index))
#define fld_clr(fld, index) fld_wrd_clr(fld_wrd(fld, index), fld_bit_index(fld, index))


#define fld_byt_comspt(fld, capct) (_s(*(fld)) + (_s((fld)->words[0]) * fld_calc_length(fld, capct)))

static inline void fld_set_bits_indices(register const fld_t *const self, size_t capct, size_t *dest) {
    register size_t curr, index;

    for (curr = 0; curr < capct; curr++) {
        _t(self->words[curr]) word = self->words[curr];

        unsigned char cnt = bits_cnt_ones(word);

        if (cnt & 1) { // if odd
            *dest = bits_trln_one(word);
            word >>= (*dest++ + 1);
            cnt--;
        }

        for (; cnt; cnt -= 2) { // even number of set bits scan from the left and from the right ...
            const unsigned char
                leadng = bits_leadn_one(word),
                trailn = bits_trlng_zrs(word);

            *dest++ = (curr * bit_size(word)) + leadng;
            *dest++ = (curr * bit_size(word)) + trailn;

//            word &= ~((1 << leadng) | (1 << trailn));

            word <<= (leadng + 1);
            word >>= (leadng + trailn + 2);
        }
    }
}


#endif