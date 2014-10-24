#ifndef __DYN_PERF_H__
#define __DYN_PERF_H__

#include "comp_utils.h"
#include "alloc.h"
#include "entry.h"
#include "hash.h"
#include "bits.h"
#include "fld.h"
#include "mem.h"
#include "entries.h"
#include "sub_table.h"

#include "vect.h"

typedef struct dyn_perf_t {
    union {
        table_t *table;
        entry_t *entry;
    } *slots;

    fld_t *entry_type;

    _t(((entry_t){}).key) coef;

    _t(
        comp_select(_s(((entry_t){}).key) < 2, (unsigned short)0,
        comp_select(_s(((entry_t){}).key) < 4, (unsigned)0,
            (size_t)0))
    ) cnt;

    union {
#       define dyn_perf_stats_t struct {unsigned char irrlvnt_bits, len_log2;}

        dyn_perf_stats_t ;
        unsigned short stats;
    };

} dyn_perf_t;

alloc_recl_sign_templs(dyn_perf);

static const struct {
    struct {
        const double expand, shrink;
    } factors;
    const _t(((dyn_perf_t){}).len_log2) initial_length_log2;
    const unsigned short initial_stats;
} dyn_perf = {
    .factors = {
        .expand = 0.6, // expand when ratio between items and length exceeds  60%
        .shrink = 0.2  // shrink when ratio between items and length subceeds 20%
    },
#define DYN_PERF_INIT_LEN_LOG2 2

    .initial_length_log2 = DYN_PERF_INIT_LEN_LOG2, //24 //bit_sz(((fld_t){}).words[0])
    .initial_stats =
        ((bit_sz(((dyn_perf_t){}).coef) - DYN_PERF_INIT_LEN_LOG2) << bit_offst(dyn_perf_stats_t, irrlvnt_bits))
      | (DYN_PERF_INIT_LEN_LOG2 << bit_offst(dyn_perf_stats_t, len_log2))

#undef DYN_PERF_INIT_LEN_LOG2
};

#define dyn_perf_length(self)       (1UL << (self)->len_log2)

#define dyn_perf_slots_capct(self)  (dyn_perf_length(self) * 2) // <<<< if number of sub tables grows beyond twice the top table ...

#define dyn_perf_capct(self)        (dyn_perf_length(self) * dyn_perf.factors.expand)
#define dyn_perf_thrshld(self)      (dyn_perf_length(self) * dyn_perf.factors.shrink)


static_inline dyn_perf_t *dyn_perf_new() {
    dyn_perf_t *const self = dyn_perf_alloc();

    self->cnt   = 0;
    self->coef  = hash_rand_coef(self->coef);
    self->stats = dyn_perf.initial_stats;

    self->slots      = (void *)entries_pow2_new(dyn_perf.initial_length_log2);
    self->entry_type = fld_pow2_new(dyn_perf.initial_length_log2);

    return self;
}

static _t(((entry_t){}).key) dyn_perf_hash(const dyn_perf_t *const self, _t(((entry_t){}).key) key) {
    return hash_univ_pow2(key, self->coef, self->irrlvnt_bits);
}

#define dyn_perf_entry_is_table(self, index)     fld_get((self)->entry_type, index)


static_inline entry_t **dyn_perf_cln_entrs(dyn_perf_t *self, entry_t **const dest, unsigned char id)
{
    typedef _t(self->entry_type->word) wrd_t;
    _t(self->cnt) curr, cnt = self->cnt;

    for (curr = fld_len(id); curr--; self->entry_type[curr].word = 0)
    {
        _t(self->slots) slots = &self->slots[curr << log2_frm_pow2[bit_sz(wrd_t)]];

        wrd_t bits;
        for (bits = self->entry_type[curr].word; bits; bits ^= (wrd_t)1 << id) {
            id = bits_leadn_one(bits);

            entrs_coll_clr(slots[id].table->slots, &dest[cnt -= slots[id].table->cnt], slots[id].table->cnt);
            sub_table_cleand_recl(slots[id].table);
            slots[id].entry = (void *)empty_entry;
        }
    }

    entrs_coll_clr(&self->slots->entry, dest, cnt);
    return dest;
}


void dyn_perf_rebuild(dyn_perf_t *self, unsigned char prev_id);


static_inline void _set_entry(dyn_perf_t *self, entry_t **slot, entry_t *entry){
    *slot = entry;
}

static_inline void _cvt_to_table(dyn_perf_t *self, entry_t **slot, entry_t *entry) {
    fld_flip(self->entry_type, slot - (_t(slot))self->slots);
    *slot = (entry_t *)table_build_2(*slot, entry);
}
static_inline void _set_entry_sub_table(table_t *self, entry_t **slot, entry_t *entry) {
    *slot = entry;
    self->cnt++;
}
static_inline void _modf_sub_table(table_t *self, entry_t **slot, entry_t *entry) {
    unsigned char upd_mask_1 = (unsigned char)(self->capct - ++self->cnt) >> (char)(CHAR_BIT - 1);
    //            ^^^^^^^^^^ 0 if table->cnt + 1 is less than table->capct
    //                       otherwise 1, (since msb would be set and moved to lsb) ...
    // the following operations will only modify states on exceeded capactiy, otherwise they do nothing ....
    self->capct        <<= upd_mask_1;
    upd_mask_1         <<= (char)1;
    self->irrlvnt_bits  -= upd_mask_1;
    self->len_log2      += upd_mask_1;

    sub_table_rehash(self, slot, entry, (self->len_log2 - upd_mask_1));
}
typedef void (*const act_t)(void *, entry_t **, entry_t *);
typedef struct oprtns_t {const act_t empty, collsn;} oprtns_t;
static struct {oprtns_t entry, table;} acts = {
    .entry = {.empty = (void *)_set_entry,           .collsn = (void *)_cvt_to_table},
    .table = {.empty = (void *)_set_entry_sub_table, .collsn = (void *)_modf_sub_table}
};


static_inline entry_t *dyn_perf_entry(const dyn_perf_t *const self, const _t(((entry_t) {}).key) key) {
    const _t(dyn_perf_hash(self, key)) id = dyn_perf_hash(self, key);

    return
        fld_get(self->entry_type, id)
            ? self->slots[id].table->slots[sub_table_hash(self->slots[id].table, key)]
            : self->slots[id].entry;
}


static_inline _t(((entry_t){}).item) dyn_perf_getitem(const dyn_perf_t *const self, const _t(((entry_t) {}).key) key) {
    return entry_query(dyn_perf_entry(self, key), key);
}

void dyn_perf_setitem (dyn_perf_t *, _t(((entry_t){}).key), _t(((entry_t){}).item));
void dyn_perf_delitem (dyn_perf_t *, _t(((entry_t){}).key));

void test_dyn_perf(
    dyn_perf_t                *self
    ,_t(((entry_t){}).key)    *const keys
    ,_t(((entry_t){}).item)   *const values
    ,const unsigned long       cnt
);

size_t dyn_perf_byt_consptn(dyn_perf_t *);

#endif