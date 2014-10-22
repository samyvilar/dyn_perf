#ifndef __FLD_H__
#define __FLD_H__

#include <stdlib.h>

#include "bits.h"
#include "comp_utils.h"

#include "mem.h"


static const unsigned char log2_frm_pow2[] = {
    [1]   = 0, [2]   = 1,
    [4]   = 2, [8]   = 3,
    [16]  = 4, [32]  = 5,
    [64]  = 6, [128] = 7,
    [256] = 8, [512] = 9
};


static_inline size_t calc_len_log2(const unsigned char a_log2, const unsigned char b_log2) {
//                         ^^^^^^^^^^^^^ returns 1 << (a_log2 - b_log2) iff a_log2 > b_log2 otherwise 1.
    return 1UL << (char)(
        (char)(a_log2 - b_log2)
     & ((char)(b_log2 - a_log2) >> (char)(CHAR_BIT - 1))
    ); // ^^^^ C compilers have a tendency of upgrading operands to int when ever either operand is smaller

}

typedef union {uword_t word;} fld_t;

fld_t *cleand_flds[comp_select(_s(void *) == 8, 44, 32)]; // 1,099,511,627,776  16 bit words ...

static_inline size_t fld_len(const unsigned char id) {
    return calc_len_log2(id, log2_frm_pow2[bit_sz(((fld_t *)0)->word)]);
}

static_inline fld_t *fld_pow2_new(unsigned id) {
    typedef _t(((fld_t *)0)->word) wrd_t;

    if (cleand_flds[id]) {
        fld_t *self = cleand_flds[id];
        cleand_flds[id] = *(fld_t **)self;
        *(fld_t **)self = (void *)0;

        return self;
    }

    return comp_select(_s(wrd_t) < _s(void *),
        calloc(
            max(calc_len_log2(id, log2_frm_pow2[bit_sz(wrd_t)]), (_s(void *)/_s(wrd_t))),
            _s(wrd_t)
        ),
        calloc(calc_len_log2(id, log2_frm_pow2[bit_sz(wrd_t)]), _s(wrd_t))
    );
}

static_inline void fld_pow2_recl_clnd(fld_t *self, const unsigned id) {
    *(fld_t **)self = cleand_flds[id];
    cleand_flds[id] = self;
}

static_inline void fld_pow2_release_alloc_blocks() {
    unsigned char id;
    fld_t *curr, *next;
    for (id = array_cnt(cleand_flds); id--; cleand_flds[id] = NULL)
        for (curr = cleand_flds[id]; curr != NULL; curr = next) {
            next = *(fld_t **)curr;
            free(curr);
        }
}



static_inline void fld_flip(fld_t *self, const size_t loc) {
    typedef _t(self[0].word) wrd_t;
    self[loc / bit_sz(wrd_t)].word ^= (wrd_t)1 << (unsigned char)(loc % bit_sz(wrd_t));
}

static_inline _t(((fld_t){}).word) fld_get(fld_t *self, const size_t loc) {
    typedef _t(self->word) wrd_t;
    return self[loc / bit_sz(wrd_t)].word & ((wrd_t)1 << (unsigned char)(loc % bit_sz(wrd_t)));
}

#define fld_byt_comspt(id) (_s(((fld_t *)NULL)->word) * fld_len(id))

static_inline size_t fld_cnt(fld_t *self, const unsigned char id) {
    size_t len, cnt = 0;
    for (len = fld_len(id); len--; cnt += bits_cnt_ones(self[len].word)) ;

    return cnt;
}

static_inline size_t *fld_entrs(fld_t *self, const unsigned char id, size_t *dest) {
    const size_t len = fld_len(id);

    _t(self->word) word;
    size_t curr, cnt;
    for (cnt = curr = 0; curr < len; curr++)
        for (word = self[curr].word; word; word &= word - (_t(word))1)
            dest[cnt++] = (curr << log2_frm_pow2[bit_sz(word)]) + bits_trlng_zrs(word);

    return dest;
}

#endif