

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
    unsigned char *words;
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


#endif