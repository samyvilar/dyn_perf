#ifndef __FLD_H__
#define __FLD_H__

#include <stdlib.h>

#include "bits.h"
#include "comp_utils.h"
#include "vect.h"
#include "sse2.h"


static const unsigned char log2_frm_pow2[] = {
    [1]   = 0, [2]   = 1,
    [4]   = 2, [8]   = 3,
    [16]  = 4, [32]  = 5,
    [64]  = 6, [128] = 7,
    [256] = 8, [512] = 9
};


static_inline size_t calc_len_log2(const size_t a_log2, const size_t b_log2) {
//                         ^^^^^^^^^^^^^ returns 1 << (a_log2 - b_log2) iff a_log2 > b_log2 otherwise 1.
    return 1UL << (
        (long long)(a_log2 - b_log2)
     & ((long long)(b_log2 - a_log2) >> (bit_sz(size_t) - 1))
    );
}

typedef union {uword_t word;} fld_t;

#define fld_null ((fld_t *)0)


extern fld_t *cleand_flds[bit_sz(void *)];

static_inline size_t fld_len(const size_t id) {
    return calc_len_log2(id, log2_frm_pow2[bit_sz(fld_null->word)]);
}


static_inline fld_t *fld_pow2_init(fld_t *self, const size_t id) {
    typedef vect_lrgst_intgl_type packd_t;
    typedef _t(self->word)        wrd_t;

    _t(vect.lrgst.intgl.ops->store) store = vect.lrgst.intgl.ops->store;
    packd_t zero = vect.lrgst.intgl.ops->setzeros();

    static const size_t compnt_cnt = _s(packd_t)/_s(fld_null->word);
    size_t cnt = fld_len(id), remndr;

    for (remndr = cnt % compnt_cnt; remndr--; self[--cnt].word = 0) ;

    for (cnt /= compnt_cnt; cnt--; store(&((packd_t *)self)[cnt], zero)) ;

    return self;
}

static_inline fld_t *fld_pow2_new(size_t id) {
    typedef _t(fld_null->word) wrd_t;

    if (cleand_flds[id]) {
        fld_t *self = cleand_flds[id];
        cleand_flds[id] = *(fld_t **)self;
        *(fld_t **)self = (void *)0;

        return self;
    }

    return fld_pow2_init(malloc(fld_len(id) * _s(wrd_t)), id);
}

static_inline void fld_pow2_recl_clnd(fld_t *self, const size_t id) {
    *(fld_t **)self = cleand_flds[id];
    cleand_flds[id] = self;
}

static_inline void fld_pow2_release_alloc_blocks() {
    size_t id;
    fld_t *curr, *next;
    for (id = array_cnt(cleand_flds); id--; cleand_flds[id] = NULL)
        for (curr = cleand_flds[id]; curr != NULL; curr = next) {
            next = *(fld_t **)curr;
            free(curr);
        }
}

static_inline size_t fld_pow2_cnt(fld_t *self, const unsigned char id) {
    size_t len = fld_len(id), cnt = 0;
    while (len--)
        cnt += bits_cnt_ones(self[len].word);

    return cnt;
}

static_inline void fld_flip(fld_t *self, const size_t loc) {
    typedef _t(self[0].word) wrd_t;
    self[loc / bit_sz(wrd_t)].word ^= (wrd_t)1 << (loc % bit_sz(wrd_t));
}

static_inline _t(fld_null->word) fld_get(fld_t *self, const size_t loc) {
    typedef _t(self->word) wrd_t;
    return self[loc / bit_sz(wrd_t)].word & ((wrd_t)1 << (loc % bit_sz(wrd_t)));
}

#define fld_byt_comspt(id) (_s(fld_null->word) * fld_len(id))


#endif