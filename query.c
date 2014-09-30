

#include "dyn_perf.h"
#include "hash.h"
#include "sub_table.h"

#include "comp_utils.h"


typedef _t(*(((dyn_perf_t *)NULL)->slots)) slot_t;

static inline void *query_table(register const slot_t *slot, const entry_key_t key) {
    slot = (void *)slot->table->slots[sub_table_hash(key, slot->table->coef, slot->table->shift_mag)];

    return comp_unlikely(((entry_t *)slot) != empty_entry) && (((entry_t *)slot)->key == key) ?
        ((entry_t *)slot)->item : NULL;
}

static inline void *query_entry(register const slot_t *const slot, const entry_key_t key) {
    register const entry_t *const entry = slot->entry;
    return (comp_likely(entry != empty_entry) && (entry->key == key)) ? entry->item : NULL;
}

void *dyn_perf_query(const dyn_perf_t *const self, register const entry_key_t key) {
    const _t(key) index = dyn_perf_hash(self, key);
    register const slot_t *const slot = &self->slots[index];

    return (dyn_perf_entry_is_table(self, index) ? query_table : query_entry)(slot, key);
}