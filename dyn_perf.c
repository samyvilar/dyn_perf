#include "dyn_perf.h"

alloc_rec_templs(dyn_perf);

typedef _t(*dyn_perf_null->slots)           slot_t;
typedef _t(*dyn_perf_null->slots->table)    table_t;


table_t *table_build_2(entry_t *a, entry_t *b);
void sub_table_rehash(table_t *self, entry_t **curr, entry_t *append, _t(dyn_perf_null->slots->table->len_log2) id);
alloc_recl_sign_templs(table);


entry_t **cleand_entries[bit_sz(entry_null->key)];


fld_t *cleand_flds[bit_sz(void *)];

static_inline void _modf_sub_table(table_t *self, entry_t **slot, entry_t *entry) {

    _t(self->cnt) upd_mask_1 = ((sub_table_capct(self) - ++self->cnt) >> (bit_sz(self->cnt) - 2)) & 2;
//                ^^^^^^^^^^ 0 if table->cnt + 1 is less than table->sub_table_capct
//                          otherwise 1, (since msb would be set and moved to lsb) ...
//    the following operations will only expand on exceeded capactiy, otherwise it simply rehashes ...
    self->len_log2 += upd_mask_1;
    sub_table_rehash(self, slot, entry, self->len_log2 - upd_mask_1);
}


static_inline void _dyn_perf_set_entry(dyn_perf_t *self, const _t(entry_null->key) key, entry_t *entry) {
    typedef _t(entry_null->key) memb_t;

    dyn_perf_packd_ids_t ids;
    dyn_perf_hashes(self, key, &ids);

    size_t curr;
    for (curr = 0; curr < array_cnt(ids.memb); curr++) {
        entry_t **dest = &self->slots[ids.memb[curr]].entry;
        if (*dest == empty_entry) {
            *dest = entry;
            return ;
        }
    }

    table_t *subs[array_cnt(ids.memb)] = {NULL};
    for (curr = 0; curr < array_cnt(ids.memb); curr++)
        if (dyn_perf_entry_is_table(self, ids.memb[curr])) {
            subs[curr] = self->slots[ids.memb[curr]].table;

            entry_t **dest = &subs[curr]->slots[hash(subs[curr], key)];
            if (*dest == empty_entry) {
                *dest = entry;
                subs[curr]->cnt++;

                return ;
            }
        }

    for (curr = 0; curr < array_cnt(subs); curr++)
        if (subs[curr] == NULL) {
            const memb_t id = ids.memb[curr];

            self->slots[id].table = table_build_2(self->slots[id].entry, entry);
            fld_flip(self->entry_type, id);

            return ;
        }

    _modf_sub_table(subs[0], &subs[0]->slots[hash(subs[0], key)], entry);
}


void dyn_perf_rebuild(dyn_perf_t *self, const _t(dyn_perf_null->len_log2) prev_id) {
    typedef _t(entry_null->key)         key_t;
    typedef _t(self->entry_type->word)  fld_memb_t;

    size_t cnt = fld_len(prev_id);
    fld_memb_t *words = &self->entry_type[cnt].word, set;
    while (cnt--)
        for (set = *--words; set; set &= set - 1) {
            table_t *sub = self->slots[(cnt << log2_frm_pow2[bit_sz(set)]) + bits_trlng_zrs(set)].table;
            entries_pow2_recl_cleand(entries_pow2_init(sub->slots, sub->len_log2), sub->len_log2);
            table_recl(sub);
        }

    entries_pow2_recl_cleand(entries_pow2_init(&self->slots->entry, prev_id), prev_id);
    self->slots  = (slot_t *)entries_pow2_new(self->len_log2);

    fld_pow2_recl_clnd(fld_pow2_init(self->entry_type, prev_id), prev_id);
    self->entry_type = fld_pow2_new(self->len_log2);

    entry_t *entries;
    for (entries = self->root; entries->next != empty_entry; entries = entries->next)
        _dyn_perf_set_entry(self, entries->next->key, entries);
}

inline int dyn_perf_contains(const dyn_perf_t *const self, const _t(entry_null->key) key) {
    dyn_perf_packd_ids_t ids;
    dyn_perf_hashes(self, key, &ids);

    size_t curr;
    for (curr = 0; curr < array_cnt(ids.memb); curr++) {
        slot_t src = self->slots[ids.memb[curr]];

        if (dyn_perf_entry_is_table(self, ids.memb[curr]))
            src.entry = src.table->slots[hash(src.table, key)];

        if (src.entry->next != empty_entry && src.entry->next->key == key)
            return 1;
    }

    return 0;
}

inline _t(entry_null->item) dyn_perf_getitem(const dyn_perf_t *const self, const _t(entry_null->key) key) {
    dyn_perf_packd_ids_t ids;
    dyn_perf_hashes(self, key, &ids);

    size_t curr;
    for (curr = 0; curr < array_cnt(ids.memb); curr++) {
        slot_t src = self->slots[ids.memb[curr]];

        if (dyn_perf_entry_is_table(self, ids.memb[curr]))
            src.entry = src.table->slots[hash(src.table, key)];

        if (src.entry->next != empty_entry && src.entry->next->key == key)
            return src.entry->next->item;
    }

    return empty_entry->item;
}

void dyn_perf_setitem(dyn_perf_t *const self, const _t(entry_null->key) key, const _t(entry_null->item) item) {
    typedef _t(entry_null->key) memb_t;

    dyn_perf_packd_ids_t ids;
    dyn_perf_hashes(self, key, &ids);

    slot_t *slots[array_cnt(ids.memb)];

    size_t curr;
    for (curr = 0; curr < array_cnt(ids.memb); curr++) {
        const memb_t id = ids.memb[curr];

        slots[curr] = &self->slots[id];
        slot_t src = *slots[curr];
        if (dyn_perf_entry_is_table(self, id)) {
            slots[curr] = (slot_t *) &src.table->slots[hash(src.table, key)];
            src.entry = slots[curr]->entry;
        }

        src.entry = src.entry->next;
        if (src.entry != empty_entry && src.entry->key == key) {
            src.entry->item = item;
            return ;
        }
    }

    entry_t *new = entry_new(key, item);
    new->next = empty_entry;

    self->last->next = new; // append to end
    new = self->last; // get predecessor
    self->last = self->last->next; // move end ponter next ...

    self->cnt++;

    for (curr = 0; curr < array_cnt(ids.memb); curr++)
        if (slots[curr]->entry == empty_entry) {
            slots[curr]->entry = new;

            const memb_t id = ids.memb[curr];
            if (dyn_perf_entry_is_table(self, id))
                self->slots[id].table->cnt++;
            return;
        }

    // locate non-sub table slot

    for (curr = 0; curr < array_cnt(ids.memb); curr++) {
        const memb_t id = ids.memb[curr];
        if (dyn_perf_entry_is_table(self, id) == 0) {
            fld_flip(self->entry_type, id);
            slots[curr]->table = table_build_2(slots[curr]->entry, new);

            if (self->cnt > dyn_perf_capct(self)) // <<<< ONLY expand on insert w/ collision!
                dyn_perf_rebuild(self, self->len_log2++);
            return ;
        }
    }

    --curr;
    _modf_sub_table(self->slots[ids.memb[curr]].table, &slots[curr]->entry, new);

    if (self->cnt > dyn_perf_capct(self))
        dyn_perf_rebuild(self, self->len_log2++); // <<<<@@ ONLY expand on insert w/ collision!
}

void dyn_perf_delitem(dyn_perf_t *const self, _t(entry_null->key) key) {
    dyn_perf_packd_ids_t ids;
    dyn_perf_hashes(self, key, &ids);

    size_t curr;
    for (curr = 0; curr < array_cnt(ids.memb); curr++) {
        const size_t id = ids.memb[curr];
        slot_t *src = &self->slots[id];
        if (dyn_perf_entry_is_table(self, id))
            src = (slot_t *)&src->table->slots[hash(src->table, key)];

        entry_t *const entry = src->entry->next;
        if (entry != empty_entry && entry->key == key) { // match found ...
            src->entry->next = entry->next; // remove entry from link list

            key = entry->next->key; // update succesor hash table slot ...
            dyn_perf_hashes(self, key, &ids);
            for (curr = 0; curr < array_cnt(ids.memb); curr++) {
                slot_t *succsr = &self->slots[ids.memb[curr]];
                if (dyn_perf_entry_is_table(self, ids.memb[curr]))
                    succsr = (slot_t *)&succsr->table->slots[hash(succsr->table, key)];

                if (succsr->entry == entry) {
                    succsr->entry = src->entry;  // make sure sucessor is using to entries predecessor
                    break ;
                }
            }

            if (self->last == entry)
                self->last = self->root;

            src->entry = empty_entry;
            entry_recl(entry);

            if (dyn_perf_entry_is_table(self, id)) {
                table_t *sub = self->slots[id].table;

                if (--sub->cnt == 1) { // if only single item, cvt sub table back to entry for faster query ...
                    src = (slot_t *)sub->slots - 1;
                    while ((++src)->entry == empty_entry) ;

                    self->slots[id].entry = src->entry;
                    src->entry = empty_entry;
                    entries_pow2_recl_cleand(sub->slots, sub->len_log2);
                    table_recl(sub);

                    fld_flip(self->entry_type, id);
                }
            }

            if (--self->cnt < dyn_perf_thrshld(self) && self->len_log2 > dyn_perf.initial_length_log2)
                dyn_perf_rebuild(self, self->len_log2--);

            return ;
        }
    }
}


size_t dyn_perf_byt_consptn(const dyn_perf_t *const self) {
    typedef _t(self->entry_type->word)  wrd_t;
    typedef _t(*dyn_perf_null->slots)   slot_t;

    size_t curr = fld_len(self->len_log2), sub_cnt = 0, lengths = 0;
    slot_t *block = &self->slots[curr << log2_frm_pow2[bit_sz(wrd_t)]];

    while (curr--) {
        block -= bit_sz(wrd_t);
        wrd_t set = self->entry_type[curr].word;

        for (sub_cnt += bits_cnt_ones(set); set; set &= set - (wrd_t)1)
            lengths += length(block[bits_trlng_zrs(set)].table);
    }

    return
          _s(*self)                             // <<<< main obj ...
        + (length(self) * _s(slot_t *))         // <<<< main table ....
        + fld_byt_comspt(self->len_log2)        // <<<< bit field for determining slot entry type

        + (self->cnt * _s(*self->slots->entry)) // <<<< entry objects

        + (sub_cnt   * _s(table_t))             // <<<< sub table objects ...
        + (lengths   * _s(slot_t *))            // <<<< sub table slots ...
        ;
}
