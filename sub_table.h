

#ifndef __SUB_TABLE_H__
#define __SUB_TABLE_H__

#include "entry.h"
#include "alloc.h"
#include "hash.h"
#include "entries.h"
#include "fld.h"


typedef struct table_t {
    union {
        struct table_t *_next;
        entry_t       **slots;
    };

    entry_key_t    coef;

    unsigned short length;

    unsigned char cnt, capct, shift_mag;
} table_t;

alloc_recl_sign_templs(table);


static inline size_t sub_table_byte_consmp(const table_t *const table){ return _s(*table); }

#define sub_table_hash hash_univ_pow2


static inline table_t *table_build_2(const entry_t *const entry_a, const entry_t *const entry_b) {
    static const _t(((table_t){}).length) initial_size  = 4;

    register table_t *const self = table_alloc();

    self->length    = initial_size;
    self->cnt       = 2;
    self->capct     = 2;
    self->coef      = hash_rand_coef(self->coef);
    self->shift_mag = bit_size(self->coef) - 2;

    register _t(self->length) loc_a, loc_b;
    for (;
           (loc_a = sub_table_hash(entry_a->key, self->coef, self->shift_mag))
        == (loc_b = sub_table_hash(entry_b->key, self->coef, self->shift_mag)) ;
        self->coef = hash_rand_coef(self->coef)
    );

    self->slots = entries_new(initial_size);

    self->slots[loc_a] = (entry_t *)entry_a;
    self->slots[loc_b] = (entry_t *)entry_b;

    return self;
}

static inline void sub_table_cleand_recl(table_t *self) {
    entries_recl_cleand(self->slots, self->length);
    table_recl(self);
}

static inline void *query_table(table_t *self, register entry_key_t key) {
    return query_entry(self->slots[sub_table_hash(key, self->coef, self->shift_mag)], key);
}


static inline void sub_table_rebuild(register table_t *const self, const entry_t *src[]) {
    typedef _t(self->length) len_t;
    static const len_t word_bit_l = bit_size(*((fld_t){}).words);

    fld_t *fld = fld_pow2_new(self->length);
    len_t curr;
    len_t hashes[self->cnt];

    for (curr = 0; curr < self->cnt; curr++) {
        hashes[curr] = sub_table_hash(src[curr]->key, self->coef, self->shift_mag);

        if (fld_get(fld, hashes[curr]))
            for (self->coef = hash_rand_coef(self->coef); curr--; fld->words[hashes[curr] / word_bit_l] = 0) ;
        else
            fld_set(fld, hashes[curr]);
    }

    for (curr = 0; curr < self->cnt; fld->words[hashes[curr++] / word_bit_l] = 0)
        self->slots[hashes[curr]] = (entry_t *)src[curr];

    fld_pow2_recl_clnd(fld, self->length);
}

static inline void sub_tbl_entrs(register table_t *const self, const entry_t *dest[]) {
    register _t(self->length) cnt = 0, curr;
    for (curr = 0; cnt < self->cnt; curr++) {
        for (; self->slots[curr] == empty_entry; curr++) ;

        dest[cnt++]       = self->slots[curr];
        self->slots[curr] = empty_entry;
    }
}

static inline void sub_tbl_expand(register table_t *const self, const entry_t *const append) {
    const entry_t *entries[self->cnt + 1];

    sub_tbl_entrs(self, entries);
    entries[self->cnt++] = append;

    entries_recl_cleand(self->slots, self->length);

    self->capct      *= 2;
    self->shift_mag  -= 2;

    self->length     *= 4;
    self->slots       = entries_new(self->length);

    sub_table_rebuild(self, entries);
}


static inline void sub_tbl_rehash(register table_t *const self, const entry_t *const append) {
    const entry_t *entries[self->cnt + 1];
    sub_tbl_entrs(self, entries);
    entries[self->cnt++] = append;

    sub_table_rebuild(self, entries);
}

static inline void sub_tbl_set_entry(register table_t *const self, const entry_t *const entry) {
//                 ^^^^^^^^^^^^^^^^^ sets an entry, (expands on capct reached or rehahsesh on collision)
    if ((self->cnt + 1) > self->capct)
        sub_tbl_expand(self, entry);
    else {
        const _t(self->length) id = sub_table_hash(entry->key, self->coef, self->shift_mag);

        if (self->slots[id] == empty_entry) {
            self->slots[id] = (entry_t *)entry;
            self->cnt++;
        } else
            sub_tbl_rehash(self, entry);
    }
}



#endif

