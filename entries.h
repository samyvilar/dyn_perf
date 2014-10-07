

#ifndef __ENTRIES_H__
#define __ENTRIES_H__

#include "comp_utils.h"
#include "entry.h"
#include "hash.h"
#include "bits.h"
#include "mem.h"
#include "vect.h"
#include "sse.h"

entry_t **cleand_entries[bit_sz(((entry_t){}).key)];

#define entries_calc_id bits_leadn_one


static_inline entry_t **entries_pow2_align_alloc(unsigned char id) {
    return malloc_align(_s(entry_t *) * ((_t(((entry_t){}).key))1 << id));
}

static_inline entry_t **entries_pow2_align_init(entry_t **const self, unsigned char id) {
    typedef lrgst_vect_ingtl_t oprn_t;

    const _t(vect.lrgst.intgl.ops->store_align) set = vect.lrgst.intgl.ops->store_align;

    const oprn_t pattern = ((oprn_t (*)(uword_t))vect.lrgst.intgl.ops->brdcst[_s(empty_entry)])((uword_t)empty_entry);
    oprn_t *const dest   = (_t(dest))self;

    const size_t times = (1 << id) / (_s(pattern)/_s(word_t));

    size_t index = times % 4;
    switch (index) {
        case 3: set(&dest[2], pattern);
        case 2: set(&dest[1], pattern);
        case 1: set(&dest[0], pattern);

        case 0: for (; index < times; index += 4) {
            set(&dest[index],     pattern);
            set(&dest[index + 1], pattern);
            set(&dest[index + 2], pattern);
            set(&dest[index + 3], pattern);
        }
    }

    return self;
}

static_inline _t(cleand_entries[0]) entries_pow2_new(unsigned char id) {
    _t(*cleand_entries) *const avbl = &cleand_entries[id];

    _t(*avbl) self = *avbl ? *avbl : entries_pow2_align_init(entries_pow2_align_alloc(id), id);
    *avbl = *avbl ? (_t(*avbl))*self : *avbl; // if there was a buffer available, remove it, otherwise do nothing ...
    *self = (_t(*self))empty_entry;

    return self;
}

static_inline void entries_pow2_recl_cleand(entry_t **self, unsigned char id) {

    entry_t ***const bucket = &cleand_entries[id];
    *(entry_t ***)self = *bucket;
    *bucket = self;
}


#endif