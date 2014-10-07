#ifndef __FLD_H__
#define __FLD_H__

#include <stdlib.h>

#include "alloc.h"
#include "bits.h"
#include "comp_utils.h"

#include "mem.h"

typedef struct fld_t {
    unsigned short *words;
    struct fld_t   *_next;
} fld_t;


alloc_recl_sign_templs(fld);

fld_t *cleand_flds[44]; // 1,099,511,627,776  16 bit words ...

#define fld_calc_length(fld, capct) (((capct) / bit_sz((fld)->words[0])) + (((capct) % bit_sz((fld)->words[0])) != 0))

static_inline fld_t *fld_pow2_new(const unsigned char id) {
    fld_t **bucket = &cleand_flds[id], *self;

    if (*bucket) {
        self = *bucket;
        *bucket = (*bucket)->_next;
        return self;
    }

    self = fld_alloc();

    const size_t capct = (size_t)1 << id;
    self->words = calloc(fld_calc_length(self, capct), _s(self->words[0]));

    return self;
}

static_inline void fld_pow2_recl_clnd(fld_t *const fld, const unsigned char id) {
    fld_t **const bucket = &cleand_flds[id];
    fld->_next = *bucket;
    *bucket = fld;
}


static const struct {
    const _t(((fld_t){}).words[0]) word_t;
    const struct {
        const _t(((fld_t){}).words[0]) mask;
        const _t(((fld_t){}).words[0]) bits_mag;
    } id;
} fld = {
    .id = {
        .mask       = bit_sz(fld.word_t) - 1,
        .bits_mag   =
            comp_select(_s(fld.word_t) == 1, 3,
            comp_select(_s(fld.word_t) == 2, 4,
            comp_select(_s(fld.word_t) == 4, 5,
            comp_select(_s(fld.word_t) == 8, 6,
                (void)0))))
    }
};

#define fld_get(self, loc)  ((self)->words[(loc) / (bit_sz((self)->words[0]))] & ((_t((self)->words[0]))1 << ((loc) % (bit_sz((self)->words[0])))))
#define fld_flip(self, loc) ((self)->words[(loc) / (bit_sz((self)->words[0]))] ^= ((_t((self)->words[0]))1 << ((loc) % (bit_sz((self)->words[0])))))

#define fld_byt_comspt(fld, capct) ((_s((fld)->words[0]) * fld_calc_length(fld, capct)) + _s(*(fld)))

#endif