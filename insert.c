#include "hash.h"
#include "dyn_perf.h"
#include "fld.h"
#include "sub_table.h"

// OS X default stack size: 8192000 bytes ...

static_inline void dyn_perf_expand(dyn_perf_t *const self) {
    const entry_t **const entries = malloc(self->cnt * _s(self->slots->entry));

    dyn_perf_cln_entrs(self, entries);

    entries_pow2_recl_cleand((entry_t **)self->slots, self->len_log2);
    fld_pow2_recl_clnd(self->entry_type, self->len_log2);

    self->slots_cnt = 0;
    self->len_log2++;
    self->shift_mag--;

    self->entry_type = fld_pow2_new(self->len_log2);
    self->slots      = (_t(self->slots))entries_pow2_new(self->len_log2);

    dyn_perf_rebuild(self, entries);

    free((void *)entries);
}

static_inline void insert_sub_table(
    dyn_perf_t *const               self,
    const _t(((entry_t){}).key)     slot_id,
    const _t(((entry_t){}).key)     key,
    const _t(((entry_t){}).item)    item
){
    table_t *const            table = self->slots[slot_id].table;

    const unsigned short      id    = sub_table_hash(key, table->coef, table->shift_mag);
    //    ^^^^^^^^^^^^^^ subtables are quite small, hence we use a short ...
    const _t(table->slots[0]) entry = table->slots[id];

    if ((entry == empty_entry) || (entry->key != key)) {
        const _t(sub_table_length(table)) prev = sub_table_length(table);

        sub_table_set_entry(table, id, entry_new(key, item));  // <<<< insert new item ...

        self->slots_cnt += (sub_table_length(table) - prev); // <<<< add extra slots, if the sub table was expanded (doubld)
        self->cnt++;
    } else
        entry->item = item; // <<<< keys match and item isn't empty, so update ...

}

static_inline void insert_entry(
    dyn_perf_t *const               self,

    const _t(((entry_t){}).key)     id,
    const _t(((entry_t){}).key)     key,
    const _t(((entry_t){}).item)    item
){
    _t(self->slots[0]) *const slot = &self->slots[id];

    if (slot->entry == empty_entry) {
        slot->entry = entry_new(key, item);
        self->cnt++;
    } else if (slot->entry->key != key) {
        fld_flip(self->entry_type, id);
        slot->table = table_build_2(slot->entry, entry_new(key, item));

        self->slots_cnt += ((unsigned short)1 << sub_table.initial_len_log2);
        self->cnt++;
    } else
        slot->entry->item = item;
//    slot->entry = (slot->entry == empty_entry) ? entry_new(key, item) : slot->entry; // <<<< if empty create new entry .
//
//    if (slot->entry->key != key) { // <<<< if collision, convert entry into sub table
//        fld_flip(self->entry_type, id);
//        slot->table = table_build_2(slot->entry, entry_new(key, item));
//        self->cnt.slots += 1 << sub_table.initial_len_log2;
//    } else
//        slot->entry->item = item; // <<<<< else its an update ...

}




void dyn_perf_setitem(
    dyn_perf_t *const            self,
    const _t(((entry_t){}).key)  key,
    const _t(((entry_t){}).item) item
) {
    const _t(dyn_perf_hash(self, key)) id = dyn_perf_hash(self, key);

    if (dyn_perf_entry_is_table(self, id))
        insert_sub_table (self, id, key, item);
    else
        insert_entry     (self, id, key, item);

    if ((self->cnt > dyn_perf_capct(self)) || (self->slots_cnt >  dyn_perf_slots_capct(self)))
        dyn_perf_expand(self);
}
