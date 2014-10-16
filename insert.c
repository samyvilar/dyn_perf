#include "hash.h"
#include "dyn_perf.h"
#include "fld.h"
#include "sub_table.h"

// OS X default stack size: 8192000 bytes ... ~ 1 million pointers on 64 bit arch ...

static void __setentry(
    void     *self,

    entry_t **other,
    entry_t *entry,
    oprtns_t *acts
) {
    const _t(acts->empty)
        setentry = (empty_entry != *other)
              ? (((*other)->key != entry->key) ? acts->collsn : acts->match)
              : acts->empty;

    setentry(self, other, entry);
}

void dyn_perf_setitem(dyn_perf_t *const self, _t(((entry_t){}).key) id, const _t(((entry_t){}).item) item)
{
    oprtns_t *actns;
    void  *obj;
    entry_t **other, *entry = entry_new(id, item);
    id = dyn_perf_hash(self, id);


    if (fld_get(self->entry_type, id)) {
        obj   = self->slots[id].table;
        other = &(self->slots[id].table->slots[sub_table_hash(self->slots[id].table, entry->key)]);
        actns = &acts.table;
    } else {
        obj   = self;
        other = &(self->slots[id].entry);
        actns = &acts.entry;
    }

    self->cnt += (*other == empty_entry || (*other)->key != entry->key);
    __setentry(obj, other, entry, actns);

    if (self->cnt > dyn_perf_capct(self)) {
        self->irrlvnt_bits--;
        dyn_perf_rebuild(self, self->len_log2++);
    }
}
