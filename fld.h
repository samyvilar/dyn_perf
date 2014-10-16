#ifndef __FLD_H__
#define __FLD_H__

#include <stdlib.h>

#include "alloc.h"
#include "bits.h"
#include "comp_utils.h"

#include "mem.h"

typedef struct fld_t {
    unsigned long *words;
    struct fld_t   *_next;
} fld_t;


alloc_recl_sign_templs(fld);

fld_t *cleand_flds[44]; // 1,099,511,627,776  16 bit words ...


static const unsigned char fld_wrd_bit_sz = bit_sz(((fld_t){}).words[0]);

static const unsigned char bit_sz_log2[9] = {[1] = 3, [2] = 4, [4] = 5, [8] = 6};

//#define fld_calc_length(capct) (((capct) / fld_wrd_bit_sz) + (((capct) % fld_wrd_bit_sz) != 0))

static_inline unsigned long fld_len(fld_t *self, unsigned char id) {
    typedef _t(self->words[0]) wrd_t;
    return 1UL << ((char)(id - bit_sz_log2[_s(wrd_t)]) & ((char)(bit_sz_log2[_s(wrd_t)] - id) >> (char)7)) ;
}

static_inline fld_t *fld_pow2_new(const unsigned id) {
    fld_t *self;

    if (cleand_flds[id]) {
        self = cleand_flds[id];
        cleand_flds[id] = self->_next;
    } else {
        self = fld_alloc();
        self->words = calloc(fld_len(self, id), _s(self->words[0]));
    }

    return self;
}

static_inline void fld_pow2_recl_clnd(fld_t *fld, const unsigned id) {
    fld->_next = cleand_flds[id];
    cleand_flds[id] = fld;
}

#define fld_word(seq, loc) ((seq)[(loc) / bit_sz((seq)[0])])


#define fld_words_get(seq, loc)  (fld_word(seq, loc) &  (_t((seq)[0]))((_t((seq)[0]))1 << ((loc) % bit_sz((seq)[0]))))
#define fld_words_flip(seq, loc) (fld_word(seq, loc) ^= (_t((seq)[0]))((_t((seq)[0]))1 << ((loc) % bit_sz((seq)[0]))))

static_inline void fld_flip(fld_t *self, const size_t loc) {
    typedef _t(self->words[0]) wrd_t;
    self->words[loc / bit_sz(wrd_t)] ^= (wrd_t)1 << (unsigned char)(loc % bit_sz(wrd_t));
}

static_inline _t(((fld_t){}).words[0]) fld_get(fld_t *self, const size_t loc) {
    typedef _t(self->words[0]) wrd_t;
    return self->words[loc / bit_sz(wrd_t)] & ((wrd_t)1 << (unsigned char)(loc % bit_sz(wrd_t)));
}

#define fld_byt_comspt(fld, id) ((_s((fld)->words[0]) * fld_len(fld, id)) + _s(*(fld)))

static_inline void fld_clr(fld_t *self, const unsigned char id) {
    unsigned long length; for (length = fld_len(self, id); length--; self->words[length] = 0) ;
}

static_inline void fld_cln_entrs(fld_t *self, const unsigned char id, unsigned long *dest) {
    const unsigned long length = fld_len(self, id);

    _t(self->words[0]) word;
    unsigned long curr, offset;
    for (offset = curr = 0; curr < length; (offset += bit_sz(word)), (self->words[curr++] = 0))
        for (word = self->words[curr]; word; word &= word - 1)
            *dest++ = offset + bits_trlng_zrs(word);
}

#endif