#include "hash.h"
#include "dyn_perf.h"
#include "fld.h"
#include "sub_table.h"

// OS X default stack size: 8192000 bytes ... ~ 1 million pointers on 64 bit arch ...

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
