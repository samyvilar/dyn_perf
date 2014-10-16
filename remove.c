
#include "comp_utils.h"
#include "dyn_perf.h"
#include "sub_table.h"
#include "entry.h"

void dyn_perf_delitem(dyn_perf_t *const self, const _t(((entry_t){}).key) key) {
    _t(key) id = dyn_perf_hash(self, key);

    _t(self->entry_type->word) is_sub_table = fld_get(self->entry_type, id);

    entry_t **other =
        is_sub_table ? &self->slots[id].table->slots[sub_table_hash(self->slots[id].table, key)]
                     : &self->slots[id].entry;

    if (*other == empty_entry || (*other)->key != key)
        return ;

    self->cnt--;
    entry_recl(*other);
    *other = (void *)empty_entry;

    if (is_sub_table && !--(self->slots[id].table->cnt)) {
        fld_flip(self->entry_type, id);
        table_recl(self->slots[id].table);
        self->slots[id].entry = (void *)empty_entry;
    }

    if ((self->len_log2 > dyn_perf.initial_length_log2) && (self->cnt < dyn_perf_thrshld(self))) {
        self->irrlvnt_bits++;
        dyn_perf_rebuild(self, self->len_log2--);
    }
}




