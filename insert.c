#include "hash.h"
#include "dyn_perf.h"
#include "fld.h"
#include "sub_table.h"


typedef _t(*(((dyn_perf_t *)NULL)->slots)) slot_t;



static inline void dyn_perf_expand(dyn_perf_t *const self) {
    // OS X default stack size: 8192000 bytes ...

    _t(self->slots->entry) *entries = malloc(self->cnt.items * _s(self->slots->entry));
    dyn_perf_entrs(self, entries);

    entries_recl_cleand((entry_t **)self->slots, self->length);
    fld_pow2_recl_clnd(self->entry_type, self->length);

    self->length *= 2;
    self->shift_mag--;

    self->capct.items = dyn_perf_calc_capct(self->length);
    self->capct.slots = dyn_perf_calc_slots_capacity(self->length);
    self->entry_type  = fld_pow2_new(self->length);

    self->cnt.slots   = 0;

    _t(self->coef)      coef = self->coef;
    _t(self->shift_mag) shft = self->shift_mag;

    register slot_t *slots = self->slots = (void *)entries_new(self->length);

    _t(self->length) index;

    _t(entries) termnl;
    for (termnl = entries + self->cnt.items; entries < termnl; entries++) {
        index = hash_univ_pow2((*entries)->key, coef, shft);
        slot_t *slot = &slots[index];

        _t(slot->table->length) new_slots = 0;

        if (slot->entry == empty_entry)
            slot->entry = *entries;
        else if (dyn_perf_entry_is_table(self, index)) {
            new_slots = slot->table->length;
            sub_tbl_set_entry(slot->table, *entries);
            new_slots = slot->table->length - new_slots;
        } else {
            slot->table = table_build_2(slot->entry, *entries);
            new_slots   = slot->table->length;
            dyn_perf_mark_entry_table(self, index);
        }

        self->cnt.slots += new_slots;
    }

    assert_with_msg(self->cnt.slots <= self->capct.slots, "failed to expand properly!");
    free(entries - self->cnt.items);
}


static inline void insert_sub_table(dyn_perf_t *const self, slot_t *const slot, const entry_key_t key, void *item) {
    _t(slot->table)  table    = slot->table;
    _t(table->slots) sub_slot = &table->slots[sub_table_hash(key, table->coef, table->shift_mag)];
    _t(*sub_slot)    entry    = *sub_slot;

    if ((entry->key == key) && comp_likely(entry != empty_entry))
        entry->item = item;
    else {
        const _t(table->length) prev = table->length;
        sub_tbl_set_entry(table, entry_new(key, item));
        self->cnt.slots += (table->length - prev);

        self->cnt.items++;
    }
}

static inline void insert_entry(dyn_perf_t *const self, slot_t *const slot, const entry_key_t key, void *const item){
    if (slot->entry == empty_entry) {
        slot->entry = entry_new(key, item);
        self->cnt.items++;
    } else if (slot->entry->key != key) {
        slot->table = table_build_2(slot->entry, entry_new(key, item));
        dyn_perf_mark_entry_table(self, slot - self->slots);

        self->cnt.items++;
        self->cnt.slots += slot->table->length;
    } else
        slot->entry->item = item;
}


void dyn_perf_insert(dyn_perf_t *self, const entry_key_t key, void *item) {
    register const _t(dyn_perf_hash(self, key)) index = dyn_perf_hash(self, key);

    if (dyn_perf_entry_is_table(self, index))
        insert_sub_table(self, &self->slots[index], key, item);
     else
        insert_entry(self, &self->slots[index], key, item);

    if (comp_unlikely((self->cnt.items > self->capct.items) || (self->cnt.slots > self->capct.slots)))
        dyn_perf_expand(self);
}
