

#include "dyn_perf.h"
#include "alloc.h"
#include "comp_utils.h"
#include "entry.h"
#include "fld.h"
#include "timed.h"
#include "sub_table.h"
#include "vect.h"
#include "mem.h"

alloc_rec_templs(dyn_perf);



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


static void _dyn_perf_set_entry(dyn_perf_t *volatile self, _t(((entry_t){}).key) id, entry_t *entry) {
    oprtns_t actions;
    entry_t **other;

    if (dyn_perf_entry_is_table(self, id)) {
        other = &self->slots[id].table->slots[sub_table_hash(self->slots[id].table, entry->key)];
        actions = acts.table;
        self = (void *)self->slots[id].table;
    } else {
        other = &(self->slots[id].entry);
        actions = acts.entry;
    }

    (*other != empty_entry ? actions.collsn : actions.empty)(self, other, entry);
}


static void dyn_perf_rebuild(dyn_perf_t *self, unsigned char prev_id)
{
    typedef _t(((entry_t){}).key)       memb_t;
    typedef lrgst_vect_ingtl_t          oprn_t;

    entry_t **const src = dyn_perf_cln_entrs(self, malloc(self->cnt * _s(self->slots->entry)), prev_id);

    entries_pow2_recl_cleand((entry_t **)self->slots, prev_id);
    fld_pow2_recl_clnd(self->entry_type, prev_id);

    self->entry_type = fld_pow2_new(self->len_log2);
    self->slots = (_t(self->slots))entries_pow2_new(self->len_log2);

    const size_t
        remndr = self->cnt % (_s(oprn_t)/_s(memb_t)),
        key_cnt = self->cnt + (remndr ? ((_s(oprn_t)/_s(memb_t)) - remndr) : 0);

    memb_t *const keys = malloc(key_cnt * _s(*keys));
    _t(self->cnt) curr;
    for (curr = self->cnt; curr--; keys[curr] = src[curr]->key) ;

    hashes(
        hashr_init(&(hashr_t){}, self->coef, self->irrlvnt_bits),
        (void *)keys,
        (void *)keys,
        key_cnt/(_s(oprn_t)/_s(memb_t))
    );

    for (curr = self->cnt; curr--; _dyn_perf_set_entry(self, keys[curr], src[curr]))
        ;

    free(keys);
    free(src);
}


void dyn_perf_delitem(dyn_perf_t *const self, const _t(((entry_t){}).key) key) {
    _t(key) id = dyn_perf_hash(self, key);

    _t(self->entry_type->word) is_sub_table = fld_get(self->entry_type, id);

    entry_t **other =
        is_sub_table
            ? &self->slots[id].table->slots[sub_table_hash(self->slots[id].table, key)]
            : &self->slots[id].entry;

    if (*other == empty_entry || (*other)->key != key)
        return ;

    entry_recl(*other);
    *other = (void *)empty_entry;

    if (is_sub_table && --(self->slots[id].table->cnt) == (_t(self->slots[id].table->cnt))1) {
        fld_flip(self->entry_type, id);
        table_t *table = self->slots[id].table;
        entrs_coll_clr(table->slots, &self->slots[id].entry, 1);
        sub_table_cleand_recl(table);
    }

    if (--self->cnt < dyn_perf_thrshld(self) && self->len_log2 > dyn_perf.initial_length_log2) {
        self->irrlvnt_bits++;
        dyn_perf_rebuild(self, self->len_log2--);
    }
}


void dyn_perf_setitem(dyn_perf_t *const self, _t(((entry_t){}).key) id, const _t(((entry_t){}).item) item)
{
    void  *obj;
    oprtns_t *actns;
    entry_t **other, *entry = entry_new(id, item);

    id = dyn_perf_hash(self, id);

    if (fld_get(self->entry_type, id)) {
        obj   = self->slots[id].table;
        other = &self->slots[id].table->slots[sub_table_hash(self->slots[id].table, entry->key)];
        actns = &acts.table;
    } else {
        obj   = self;
        other = &self->slots[id].entry;
        actns = &acts.entry;
    }

    if (empty_entry == *other) {
        actns->empty(obj, other, entry);
        self->cnt++;
        return ;
    }

    if ((*other)->key == entry->key) {
        (*other)->item = entry->item;
        entry_recl(entry);
        return ;
    }

    actns->collsn(obj, other, entry);
    if (++self->cnt > dyn_perf_capct(self)) { // expand only on collisions ...
        self->irrlvnt_bits--;
        dyn_perf_rebuild(self, self->len_log2++);
    }
}



size_t dyn_perf_byt_consptn(dyn_perf_t *self) {
    size_t curr, cnt, lengths,
        total = _s(*self)
            + dyn_perf_length(self) * _s(self->slots[0])
            + self->cnt * _s(entry_t)
            + fld_byt_comspt(self->len_log2)
            ;

    for (curr = cnt = lengths = 0; curr < dyn_perf_length(self); curr++) // subtables ...
        for (; dyn_perf_entry_is_table(self, curr); cnt++)
            lengths += sub_table_length(self->slots[curr++].table);

    return total + (cnt * _s(*(self->slots->table))) + (lengths * _s(self->slots->table->slots[0]));
}
