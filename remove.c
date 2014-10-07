

#include "comp_utils.h"
#include "dyn_perf.h"
#include "sub_table.h"
#include "entry.h"


static_inline void dyn_perf_shrink(dyn_perf_t *const self)  {
    const entry_t **const entries = malloc(self->cnt * _s(*entries));

    dyn_perf_cln_entrs(self, entries);

    entries_pow2_recl_cleand((entry_t **)self->slots, self->len_log2);
    fld_pow2_recl_clnd(self->entry_type, self->len_log2);

    self->slots_cnt = 0;
    self->len_log2--;
    self->shift_mag++;

    self->entry_type  = fld_pow2_new(self->len_log2);
    self->slots       = (void *)entries_pow2_new(self->len_log2);


    dyn_perf_rebuild(self, entries);
    free((void *)entries);
}


static_inline void dyn_perf_remove_entry(
    dyn_perf_t *const           self,
    const _t(((entry_t){}).key) key,
    const _t(((entry_t){}).key) id
) {
    const _t(self->slots) slot = &self->slots[id];

    if (slot->entry->key == key) {
        self->cnt -= (slot->entry != empty_entry); // make sure its not a collision on empty key entry ...

        entry_recl((entry_t *)slot->entry);
        slot->entry = (entry_t *)empty_entry;
    }
}


static_inline void dyn_pref_remove_from_sub_table(
    dyn_perf_t *const           self,
    const _t(((entry_t){}).key) key,
    const _t(((entry_t){}).key) id
) {
    table_t             *const table = self->slots[id].table;
    _t(table->slots[0]) *const slot  = &(table->slots[sub_table_hash(key, table->coef, table->shift_mag)]);

    if ((*slot)->key == key) {
        entry_recl(*slot);

        const int is_not_empty = *slot != empty_entry;
        self->cnt -= is_not_empty; // only subtract if entry is empty ... (collision with empty_entry key) ...
        table->cnt -= is_not_empty;

        *slot = (entry_t *)empty_entry;
        if (table->cnt == 0) { // if zero recycle table ...
            self->slots_cnt -= sub_table_length(table);

            sub_table_cleand_recl(table);

            self->slots[id].entry = (entry_t *) empty_entry;
            fld_flip(self->entry_type, id);
        }
    }
}

void dyn_perf_delitem(dyn_perf_t *const self, const _t(((entry_t){}).key) key) {
    const _t(key) id = dyn_perf_hash(self, key);

    if (dyn_perf_entry_is_table(self, id))
        dyn_pref_remove_from_sub_table  (self, key, id);
    else
        dyn_perf_remove_entry           (self, key, id);

    if ((self->len_log2 > dyn_perf.initial_length_log2) && (self->cnt < dyn_perf_thrshld(self)))
        dyn_perf_shrink(self);
}



