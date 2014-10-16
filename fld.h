#ifndef __FLD_H__
#define __FLD_H__

#include <stdlib.h>

#include "bits.h"
#include "comp_utils.h"

#include "mem.h"


static const unsigned char log2[] = {
    [1]   = 0, [2]   = 1,
    [4]   = 2, [8]   = 3,
    [16]  = 4, [32]  = 5,
    [64]  = 6, [128] = 7,
    [256] = 8, [512] = 9
};


static_inline unsigned long calc_len_log2(const unsigned char a_log2, const unsigned char b_log2) {
//                         ^^^^^^^^^^^^^ returns 1 << (a_log2 - b_log2) iff a_log2 > b_log2 otherwise 1.
    return 1UL << (char)(
        (char)(a_log2 - b_log2)
     & ((char)(b_log2 - a_log2) >> (char)(CHAR_BIT - 1))
    ); // ^^^^ C compilers have a tendency of upgrading operands to int when ever either operand is smaller
}

typedef union fld_t {
    unsigned long word;
    void         *_next;
} fld_t;

fld_t *cleand_flds[comp_select(sizeof(uword_t) == 8, 44, 32)]; // 1,099,511,627,776  16 bit words ...

static_inline size_t fld_len(fld_t *self, unsigned char id) {
    return calc_len_log2(id, log2[bit_sz(self->word)]);
}
static_inline fld_t *fld_pow2_new(const unsigned id) {
    if (cleand_flds[id]) {
        fld_t *self = cleand_flds[id];
        cleand_flds[id] = self->_next;
        self->_next = (void *)0;

        return self;
    }

    return calloc(calc_len_log2(id, log2[bit_sz(((fld_t){}).word)]), _s(((fld_t){}).word));
}

static_inline void fld_pow2_recl_clnd(fld_t *self, const unsigned id) {
    self->_next = cleand_flds[id];
    cleand_flds[id] = self;
}

static_inline void fld_flip(fld_t *self, const size_t loc) {
    typedef _t(self[0].word) wrd_t;
    self[loc / bit_sz(wrd_t)].word ^= (wrd_t)1 << (unsigned char)(loc % bit_sz(wrd_t));
}

static_inline _t(((fld_t){}).word) fld_get(fld_t *self, const size_t loc) {
    typedef _t(self->word) wrd_t;
    return self[loc / bit_sz(wrd_t)].word & ((wrd_t)1 << (unsigned char)(loc % bit_sz(wrd_t)));
}

#define fld_byt_comspt(fld, id) (_s((fld)->word) * fld_len(fld, id))

static_inline void fld_clr(fld_t *self, const unsigned char id) {
    size_t curr, len = fld_len(self, id);
    for (curr = 0; curr < len; self[curr++].word = 0) ;
}

static_inline void fld_cln_entrs(fld_t *self, const unsigned char id, unsigned long *dest) {
    const size_t length = fld_len(self, id);

    _t(self->word) word;
    unsigned long curr;
    for (curr = 0; curr < length; self[curr].word = 0)
        for (word = self[curr].word; word; word &= word - 1)
            *dest++ = (curr << log2[bit_sz(word)]) + bits_trlng_zrs(word);
}

#endif