#ifndef __DYN_PERF_H__
#define __DYN_PERF_H__

#include "alloc.h"
#include "entry.h"
#include "hash.h"
#include "bits.h"
#include "fld.h"
#include "mem.h"
#include "entries.h"
#include "sub_table.h"

typedef entry_key_t dyn_perf_len_t;

typedef struct dyn_perf_t {
    union {
        struct dyn_perf_t *_next;
        union {
            table_t *table;
            entry_t *entry;
        } *slots;
    };

    dyn_perf_len_t  coef;
    unsigned short  shift_mag;

    fld_t         *entry_type;

    dyn_perf_len_t length;

    struct {dyn_perf_len_t items, slots;} cnt;
    struct {dyn_perf_len_t items, slots;} capct;

} dyn_perf_t;

alloc_recl_sign_templs(dyn_perf);


static const struct {
    const dyn_perf_len_t load;
    const double         resize;
} factors = {.load = 2, .resize = .6};

static inline dyn_perf_len_t dyn_perf_calc_capct(const dyn_perf_len_t size) {
    return size * factors.resize; //factors.load;
}
static inline dyn_perf_len_t dyn_perf_calc_slots_capacity(const dyn_perf_len_t size) {
    // c resizing factor, some value strictly greater than zero
    // M = (1 + c) * max(count, 4);  // capacity
    // s(M) is the number of subsets to partition on
    // 32 * M^2 / s(M) + 4 * M // <<< max number of slots for sub tables ...
    const dyn_perf_len_t M = size * factors.resize;

    return M * (((32/size) * M) + 4); // TODO: check for overflows, size cannot exceed 27 words ...
}


static inline dyn_perf_t *dyn_perf_new() {
    static const dyn_perf_len_t size = bit_size(((fld_t *)NULL)->words[0]);

    dyn_perf_t *const self = dyn_perf_alloc();

    self->cnt.items = 0;
    self->cnt.slots = 0;

    self->length    = size;

    self->slots      = (void *)entries_new(size);
    self->entry_type = fld_pow2_new(size);

    self->capct.items = dyn_perf_calc_capct(size);
    self->capct.slots = dyn_perf_calc_slots_capacity(size);

    init_hash(self);

    return self;
}

static inline dyn_perf_len_t dyn_perf_hash(const dyn_perf_t *const self, const entry_key_t key) {
    return hash_univ_pow2(key, self->coef, self->shift_mag);
}
static inline int dyn_perf_entry_is_table(const dyn_perf_t *const self, dyn_perf_len_t index) {
    return fld_get(self->entry_type, index);
}
static inline void dyn_perf_mark_entry_table(dyn_perf_t *const self, dyn_perf_len_t index) {
    fld_set(self->entry_type, index);
}
static inline void dyn_perf_mark_entry(dyn_perf_t *const self, dyn_perf_len_t index) {
    fld_clr(self->entry_type, index);
}

static inline void dyn_perf_recl_prts(register dyn_perf_t *const self) {
    register _t(self->slots) slots = self->slots;

    for (slots = self->slots; self->cnt.items; slots++)
        if (slots->entry == empty_entry)
            continue;
        else if (dyn_perf_entry_is_table(self, (slots - self->slots))) {
            self->cnt.items -= slots->table->cnt;

            register _t(slots->table->slots) entries;
            for (entries = slots->table->slots; slots->table->cnt; entries++)
                if (comp_unlikely(*entries != empty_entry)) {
                    entry_recl(*entries);
                    *entries = empty_entry;
                    slots->table->cnt--;
                }

            sub_table_cleand_recl(slots->table);

            slots->entry = empty_entry;
            dyn_perf_mark_entry(self, (slots - self->slots));
        } else {
            self->cnt.items--;
            entry_recl(slots->entry);
            slots->entry = empty_entry;
        }

    entries_recl_cleand((entry_t **)self->slots, self->length);
    fld_pow2_recl_clnd(self->entry_type, self->length);
    dyn_perf_recl(self);
}


static inline void dyn_perf_entrs(register dyn_perf_t *const self, const entry_t *dest[]) {
    _t(self->length) cnt = self->cnt.items, ids;

    for (ids = 0; cnt; ids++) {
        for (; self->slots[ids].entry == empty_entry; ids++) ;

        void *obj = self->slots[ids].entry;

        if (dyn_perf_entry_is_table(self, ids)) {
            sub_tbl_entrs((table_t *)obj, &dest[(cnt -= ((table_t *)obj)->cnt)]);
            sub_table_cleand_recl((table_t *)obj);
        } else
            dest[--cnt] = obj;

        dyn_perf_mark_entry(self, ids);
        self->slots[ids].entry = empty_entry;
    }
}

static inline void *dyn_perf_query(dyn_perf_t *self, register entry_key_t key) {
    _t(key) index = dyn_perf_hash(self, key);

    return dyn_perf_entry_is_table(self, index)
        ? query_table(self->slots[index].table, key)
        : query_entry(self->slots[index].entry, key)
        ;
}


void dyn_perf_insert(dyn_perf_t *self, entry_key_t key, void *item);
void dyn_perf_remove(dyn_perf_t *self, entry_key_t key);


void test_dyn_perf(
    dyn_perf_t      *self
    ,entry_key_t    *const keys
    ,void          **const values
    ,const unsigned long   cnt
);


#endif