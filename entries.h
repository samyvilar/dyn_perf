

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


static_inline void entries_release_alloc_blocks() {
    unsigned char id;
    entry_t **curr, **next;
    for (id = array_cnt(cleand_entries); id--; cleand_entries[id] = NULL) {
        for (curr = cleand_entries[id]; curr != NULL; curr = next) {
            next = *(entry_t ***) curr;
            free(curr);
        }
    }
}


static_inline entry_t **entries_pow2_init(entry_t **const self, const unsigned char id) {
    typedef lrgst_vect_ingtl_t oprn_t;

    void (*const store)(oprn_t *, oprn_t)   = vect.lrgst.intgl.ops->store;
    oprn_t (*const brdcst)(_t(empty_entry)) = vect.lrgst.intgl.ops->brdcst[_s(empty_entry)];

    const oprn_t pattrn = brdcst(empty_entry);
    size_t curr;
    for (curr = calc_len_log2(id, log2_frm_pow2[_s(oprn_t)/_s(word_t)]); curr--; store(&((oprn_t *)self)[curr], pattrn))
        ;

    return self;
}

static_inline entry_t **entries_pow2_new(const unsigned char len_log2)
{
    if (cleand_entries[len_log2]) {
        entry_t **self = cleand_entries[len_log2];
        cleand_entries[len_log2] = *(entry_t ***)self;
        *self = (void *)empty_entry;
        return self;
    }

    return entries_pow2_init(malloc(_s(entry_t *) * (1ULL << len_log2)), len_log2);
}

static_inline void entries_pow2_recl_cleand(entry_t **self, const unsigned char len_log2) {
    *(entry_t ***)self = cleand_entries[len_log2];
    cleand_entries[len_log2] = self;
}

#endif