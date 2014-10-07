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
#include "sse.h"


typedef _t(((entry_t){}).key) dyn_perf_len_t;

typedef struct dyn_perf_t {
    union {
        struct dyn_perf_t *_next;
        union {
            table_t *table;
            entry_t *entry;
        } *slots;
    };

    fld_t *entry_type;

    dyn_perf_len_t coef;

    unsigned char
        shift_mag,
        len_log2;

    dyn_perf_len_t cnt;

    dyn_perf_len_t slots_cnt;

} dyn_perf_t;

alloc_recl_sign_templs(dyn_perf);

#define dyn_perf_length(self) ((_t((self)->slots->entry->key))1 << (self)->len_log2)

static const struct {
    struct {
        const dyn_perf_len_t load;
        const double expand, shrink;
    } factors;
    const _t(((dyn_perf_t){}).len_log2) initial_length_log2;
} dyn_perf = {
    .factors = {
        .expand = 0.6, // expand when ratio between items and length exceeds    60%
        .shrink = 0.2 // shrink when ratio between items and length drops below 20%
    },
    .initial_length_log2 = 20 //bit_sz(((fld_t){}).words[0])
};

static_inline dyn_perf_len_t dyn_perf_calc_capct(const dyn_perf_len_t size) {
    return size * dyn_perf.factors.expand;
}


#define dyn_perf_slots_capct(self)  (dyn_perf_length(self) * 2) // <<<< if number of sub tables grows beyond twice the top table ...

#define dyn_perf_capct(self)        (dyn_perf_length(self) * dyn_perf.factors.expand)
#define dyn_perf_thrshld(self)      (dyn_perf_length(self) * dyn_perf.factors.shrink)


static_inline dyn_perf_t *dyn_perf_new() {
    dyn_perf_t *const self = dyn_perf_alloc();

    self->cnt       = 0;
    self->slots_cnt = 0;

    self->len_log2   = dyn_perf.initial_length_log2;
    self->slots      = (void *)entries_pow2_new(dyn_perf.initial_length_log2);
    self->entry_type = fld_pow2_new(dyn_perf.initial_length_log2);

    self->coef       = hash_rand_coef((self)->coef);
    self->shift_mag  = bit_sz(self->coef) - dyn_perf.initial_length_log2;

    return self;
}

#define dyn_perf_shft_mag(self) ((self)->shift_mag)


static_inline dyn_perf_len_t dyn_perf_hash(const dyn_perf_t *const self, const _t(((entry_t){}).key) key) {
    return hash_univ_pow2(key, self->coef, dyn_perf_shft_mag(self));
}

#define dyn_perf_entry_is_table(self, index)     fld_get((self)->entry_type, index)


static_inline void dyn_perf_recl_prts(register dyn_perf_t *const self) {
    typedef  _t(self->slots->table)      table_t;
    register _t(self->slots->entry->key) ids;

    for (ids = 0; self->cnt; ids++) {
        for (; self->slots[ids].entry == empty_entry; ids++) ;

        const int is_table = dyn_perf_entry_is_table(self, ids);

        _t(self->slots[0]) *const slot = &self->slots[ids];
        self->cnt -= is_table ? slot->table->cnt : 1;

        if (is_table) {
            sub_table_clean_prts_recl(slot->table);
            fld_flip(self->entry_type, ids);
        } else
            entry_recl(slot->entry) ;

        slot->entry = (entry_t *)empty_entry;
    }

    entries_pow2_recl_cleand((entry_t **)self->slots, self->len_log2);
    fld_pow2_recl_clnd(self->entry_type, self->len_log2);

    dyn_perf_recl(self);
}


static_inline void dyn_perf_cln_entrs(register dyn_perf_t *const self, const entry_t *dest[]) {
    _t(self->cnt) cnt, ids;
    for (ids = cnt = 0; cnt < self->cnt; ids++) {
        for (; self->slots[ids].entry == empty_entry; ids++) ;

        if (fld_get(self->entry_type, ids)) {
            fld_flip(self->entry_type, ids);

            const _t(self->slots[ids].table) table = self->slots[ids].table;

            sub_table_entrs(table, &dest[cnt]);
            cnt += table->cnt;
            sub_table_cleand_recl(table);
        } else
            dest[cnt++] = (entry_t *)self->slots[ids].entry;

        self->slots[ids].entry = (entry_t *)empty_entry;
    }
}
static_inline void _dyn_perf_set_entry(
    dyn_perf_t      *const      self,
    const _t(((entry_t){}).key) id,
    const entry_t   *const      entry
) {
    _t(self->slots[0]) *const slot = &self->slots[id];
    slot->entry = (void *)((slot->entry == empty_entry) ? entry : slot->entry);

    if (slot->entry != entry) {
        if (dyn_perf_entry_is_table(self, id)) {
            const unsigned short prev = sub_table_length(slot->table);

            sub_table_set_entry(slot->table, sub_table_hash(entry->key, slot->table->coef, slot->table->shift_mag), entry);
            self->slots_cnt += sub_table_length(slot->table) - prev;
        } else {
            slot->table = table_build_2(slot->entry, entry);
            self->slots_cnt += ((unsigned short)1 << sub_table.initial_len_log2);
            fld_flip(self->entry_type, id);
        }
    }
}

static_inline void dyn_perf_rebuild(dyn_perf_t *const self, const entry_t *src[]) {
    typedef _t(((entry_t){}).key)       memb_t;
    typedef _t(self->cnt)               cnt_t;
    typedef lrgst_vect_ingtl_t          oprn_t;
    typedef oprn_t (*const bin_t)(oprn_t, oprn_t);

    const _t(vect.lrgst.intgl.ops) op = vect.lrgst.intgl.ops;

    bin_t
        mul     = op->mul[_s(memb_t)],
        rshft   = op->rshft_lgcl[_s(memb_t)];

    const oprn_t
        coef    = ((oprn_t (*)(memb_t))op->brdcst[_s(memb_t)])(self->coef),
        id_mag  = ((oprn_t (*)(long long))op->brdcst[_s(long long)])((long long)self->shift_mag);

    cnt_t curr;
    static const size_t cnt = (_s(oprn_t)/_s(memb_t));
    for (curr = 0; curr < (self->cnt % cnt); curr++)
        _dyn_perf_set_entry(self, dyn_perf_hash(self, src[curr]->key), src[curr]);

    oprn_t ids;
    cnt_t compnt;
    for ( ; curr < self->cnt; curr += cnt) {
        for (compnt = 0; compnt < cnt; compnt++)
            ((memb_t *)&ids)[compnt] = src[curr + compnt]->key;

        for (ids = rshft(mul(ids, coef), id_mag); compnt--;
             _dyn_perf_set_entry(self, ((memb_t *)&ids)[compnt], src[curr + compnt])) ;
    }
    assert_with_msg(self->slots_cnt <= dyn_perf_slots_capct(self), "failed to rebuild properly!");
}


static_inline _t(((entry_t){}).item) dyn_perf_query(
    const dyn_perf_t *const              self,
    register const _t(((entry_t){}).key) key
) {
    const _t(dyn_perf_hash(self, key)) id = dyn_perf_hash(self, key);

    return dyn_perf_entry_is_table(self, id)
         ? sub_table_query(self->slots[id].table, key)
         : entry_query(self->slots[id].entry, key);
}


void dyn_perf_setitem (dyn_perf_t *const, const _t(((entry_t){}).key), const _t(((entry_t){}).item));
void dyn_perf_delitem (dyn_perf_t *const, const _t(((entry_t){}).key));

void test_dyn_perf(
    dyn_perf_t                *const self
    ,_t(((entry_t){}).key)    *const keys
    ,_t(((entry_t){}).item)   *const values
    ,const unsigned long       cnt
);

#endif