#include "hash.h"
#include "dyn_perf.h"
#include "fld.h"
#include "sub_table.h"


typedef _t(*(((dyn_perf_t *)NULL)->slots)) slot_t;


// OS X default stack size: 8192000 bytes ...


static inline void dyn_perf_expand(register dyn_perf_t *const self) {
    const entry_t **const entries = malloc(self->cnt.items * _s(self->slots->entry));

    dyn_perf_entrs(self, entries);

    entries_recl_cleand((entry_t **)self->slots, self->length);
    fld_pow2_recl_clnd(self->entry_type, self->length);

    self->cnt.slots = 0;
    self->shift_mag--;
    self->length   *= 2;

    self->capct.items = dyn_perf_calc_capct             (self->length);
    self->capct.slots = dyn_perf_calc_slots_capacity    (self->length);
    self->entry_type  = fld_pow2_new                    (self->length);
    self->slots       = (_t(self->slots))entries_new    (self->length);

    _t(self->length) curr;
    for (curr = 0; curr < self->cnt.items; curr++) {
        const _t(self->length) id   = hash_univ_pow2(entries[curr]->key, self->coef, self->shift_mag);
        slot_t  *const         slot = &self->slots[id];

        if (slot->entry == empty_entry)
            slot->entry = (entry_t *)entries[curr];
        else if (dyn_perf_entry_is_table(self, id)) {
            const _t(slot->table->length) prev = slot->table->length;

            sub_tbl_set_entry(slot->table, entries[curr]);

            self->cnt.slots += slot->table->length - prev;
        } else {
            slot->table = table_build_2(slot->entry, entries[curr]);

            dyn_perf_mark_entry_table(self, id);

            self->cnt.slots += slot->table->length;
        }
    }

    assert_with_msg(self->cnt.slots <= self->capct.slots, "failed to expand properly!");
    free((void *)entries);
}


static inline void insert_sub_table(dyn_perf_t *const self, table_t *const table, const entry_key_t key, void *const item)
{
    const _t(table->slots) slot = &table->slots[sub_table_hash(key, table->coef, table->shift_mag)];

    if (((*slot)->key != key) || (*slot == empty_entry)) { // if keys match, make sure its not default empty key
        const _t(table->length) prev = table->length;
        sub_tbl_set_entry(table, entry_new(key, item));
        self->cnt.slots += (table->length - prev);

        self->cnt.items++;
    } else
        (*slot)->item = item; // non-empty entry with matchin keys (update) ...
}

static inline void insert_entry(dyn_perf_t *const self, slot_t *const slot, const entry_key_t key, void *const item){
    if (slot->entry == empty_entry) {
        slot->entry = entry_new(key, item);

        self->cnt.items++;
    } else if (slot->entry->key != key) {
        slot->table = table_build_2(slot->entry, entry_new(key, item));
        dyn_perf_mark_entry_table(self, slot - self->slots);
        self->cnt.slots += slot->table->length;

        self->cnt.items++;
    } else
        slot->entry->item = item;
}


void dyn_perf_insert(dyn_perf_t *self, const entry_key_t key, void *const item) {
    const _t(dyn_perf_hash(self, key)) index = dyn_perf_hash(self, key);

    if (dyn_perf_entry_is_table(self, index))
        insert_sub_table(self, self->slots[index].table, key, item);
    else
        insert_entry(self, &self->slots[index], key, item);

    if ((self->cnt.items > self->capct.items) || (self->cnt.slots > self->capct.slots))
        dyn_perf_expand(self);
}
