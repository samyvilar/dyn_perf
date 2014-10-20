
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

    entry_recl(*other);
    *other = (void *)empty_entry;

    if (is_sub_table && --(self->slots[id].table->cnt) == (_t(self->slots[id].table->cnt))1) {
        fld_flip(self->entry_type, id);
        table_t *table = self->slots[id].table;
        entrs_coll_clr(table->slots, &self->slots[id].entry, 1);
        table_recl(table);
    }

    if (--self->cnt < dyn_perf_thrshld(self) && self->len_log2 > dyn_perf.initial_length_log2) {
        self->irrlvnt_bits++;
        dyn_perf_rebuild(self, self->len_log2--);
    }
}




