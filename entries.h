

#ifndef __ENTRIES_H__
#define __ENTRIES_H__

#include "comp_utils.h"
#include "entry.h"
#include "hash.h"
#include "bits.h"
#include "vect.h"
#include "fld.h"


extern entry_t **cleand_entries[bit_sz(entry_null->key)];

static_inline void entries_release_alloc_blocks() {
    size_t id;
    entry_t **curr, **next;
    for (id = array_cnt(cleand_entries); id--; cleand_entries[id] = NULL)
        for (curr = cleand_entries[id]; NULL != curr; curr = next) {
            next = *(entry_t ***)curr;
            free(curr);
        }
}


static_inline entry_t **entries_pow2_init(entry_t **const self, const size_t id) {
    typedef vect_lrgst_intgl_type oprn_t;

    const _t(vect.lrgst.intgl.ops->store) store = vect.lrgst.intgl.ops->store;
    oprn_t (*const brdcst)(_t(empty_entry)) = vect.lrgst.intgl.ops->brdcst[_s(empty_entry)];

    const oprn_t pattrn = brdcst(empty_entry);

    size_t curr = (1UL << id)/(_s(oprn_t)/_s(empty_entry));

    while (curr--)
        store(&((oprn_t *)self)[curr], pattrn);

    return self;
}

static_inline entry_t **entries_pow2_new(const size_t len_log2) {
    if (cleand_entries[len_log2]) {
        entry_t **self = cleand_entries[len_log2];
        cleand_entries[len_log2] = *(entry_t ***)self;
        *self = empty_entry;
        return self;
    }

    return entries_pow2_init(malloc(_s(entry_t *) * (1ULL << len_log2)), len_log2);
}


static_inline void entries_pow2_recl_cleand(entry_t **self, const unsigned len_log2) {
    *(entry_t ***)self = cleand_entries[len_log2];
    cleand_entries[len_log2] = self;
}

#endif