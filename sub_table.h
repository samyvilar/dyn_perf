

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
    unsigned long long coef;

    unsigned short length;

    unsigned char capct, cnt;
    unsigned short shift_mag;
} table_t;

alloc_recl_sign_templs(table);

#define sub_table_hash(key, coef, shft_mag) (((key) * (coef)) >> (shft_mag))


static inline table_t *table_build_2(entry_t *const entry_a, entry_t *const entry_b) {
    table_t *self = table_alloc();
    const _t(self->length) initial_size  = 16;

    self->length = initial_size;
    self->cnt    = 2;
    self->capct  = 4;

    self->slots = entries_new(initial_size);
    init_hash(self);

    _t(self->coef)      coef = self->coef;
    _t(self->shift_mag) shft = self->shift_mag;

    const _t(entry_a->key)
        key_a = entry_a->key,
        key_b = entry_b->key;

    for ( ;
        comp_unlikely(sub_table_hash(key_a, coef, shft) == sub_table_hash(key_b, coef, shft));
        coef = hash_rand_coef(coef)
    ) ;


    self->coef = coef;

    self->slots[sub_table_hash(key_a, coef, shft)] = entry_a;
    self->slots[sub_table_hash(key_b, coef, shft)] = entry_b;

    return self;
}



static inline void sub_tbl_rebuild(table_t *table, entry_t **entries) {
    _t(table->length) hashes[table->cnt], *hash = hashes;

    _t(table->coef)        coef = table->coef;
    _t(table->shift_mag)   shft = table->shift_mag;

    fld_t *fld = fld_pow2_new(table->length);

    register _t(entries)
        curr, terminal = entries + table->cnt;

    for (curr = entries; curr < terminal; hash++) {
        register _t(*hash) index = *hash = sub_table_hash((*curr++)->key, coef, shft);

        if (fld_get(fld, index)) {
            for (table->coef = coef = hash_rand_coef(coef); hash >= hashes; hash--)
                fld->words[*hash / bit_size(fld->words[0])] = 0;
            curr = entries;
        } else
            fld_set(fld, index);
    }

    while (--curr >= entries) {
        table->slots[*--hash] = *curr;
        fld->words[*hash / bit_size(fld->words[0])] = 0;
    }

    fld_pow2_recl_clnd(fld, table->length);
}


static inline void sub_tbl_entrs(table_t *table, register entry_t **dest) {
    _t(dest) termnl = dest + table->cnt;
    _t(table->slots) slots;
    for (slots = table->slots; dest < termnl; slots++) {
        if (*slots != empty_entry)
            *dest++ = *slots;
        *slots = empty_entry;
    }
}

static inline void sub_tbl_expand(table_t *self, entry_t *append) {
    self->capct        *= 2;
    self->shift_mag    -= 2;

    entry_t *entries[self->cnt + 1];

    sub_tbl_entrs(self, entries);

    entries[self->cnt++] = append;

    entries_recl_cleand(self->slots, self->length);

    self->slots = entries_new((self->length *= 4));

    sub_tbl_rebuild(self, entries);
}


static inline void sub_tbl_rehash(table_t *self, entry_t *append) {
    entry_t *entries[self->cnt + 1];

    sub_tbl_entrs(self, entries);
    entries[self->cnt++] = append;

    sub_tbl_rebuild(self, entries);
}


static inline void sub_tbl_set_entry(table_t *self, entry_t *entry) {
    // sets an entry, (expands on capct reached) (rehahsesh on collision)
    _t(entry) *slot = &self->slots[sub_table_hash(entry->key, self->coef, self->shift_mag)];

    if ((self->cnt + 1) > self->capct)
        sub_tbl_expand(self, entry);
    else if (*slot == empty_entry) {
        *slot = entry;
        ++(self->cnt);
    } else
        sub_tbl_rehash(self, entry);
}




#endif

