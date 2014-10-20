

#ifndef __ENTRIES_H__
#define __ENTRIES_H__

#include "comp_utils.h"
#include "entry.h"
#include "hash.h"
#include "bits.h"
#include "mem.h"
#include "vect.h"
#include "fld.h"

entry_t **cleand_entries[comp_select(_s(((entry_t){}).key) == 8, 45, bit_sz(((entry_t){}).key))];
// 2^44 ==  1.7 trillion entries


static_inline entry_t **entries_pow2_align_alloc(const unsigned char id) {
    return malloc_align(_s(entry_t *) * ((size_t)1 << id));
}

static_inline entry_t **entries_pow2_align_init(entry_t **self, const unsigned char id) {
    typedef lrgst_vect_ingtl_t oprn_t;

    void (*const store)(oprn_t *, oprn_t)     = vect.lrgst.intgl.ops->store_align;
    oprn_t (*const brdcst)(_t(empty_entry)) = vect.lrgst.intgl.ops->brdcst[_s(empty_entry)];

    const oprn_t pattrn = brdcst(empty_entry);
    size_t len;
    for (len = calc_len_log2(id, log2_frm_pow2[_s(oprn_t)/_s(word_t)]); len--; store(&((oprn_t *)self)[len], pattrn)) ;

    return self;
}

static_inline _t(cleand_entries[0]) entries_pow2_new(const unsigned id)
{
    _t(cleand_entries[0]) self =
        cleand_entries[id] ? cleand_entries[id]
                           : entries_pow2_align_init(entries_pow2_align_alloc(id), id);

    cleand_entries[id] = cleand_entries[id] ? (void *)*self : cleand_entries[id];
    //                   ^^^^^^^^^^^^^^^^^^ if there was a block available, remove it, otherwise do nothing ...

    *self = (void *)empty_entry;
    return self;
}

static_inline void entries_pow2_recl_cleand(entry_t **self, const unsigned char id) {
    *(entry_t ***)self = cleand_entries[id];
    cleand_entries[id] = self;
}


#endif