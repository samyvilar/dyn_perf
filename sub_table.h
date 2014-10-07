

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

    _t(((entry_t){}).key) coef;

    unsigned char
        len_log2,
        cnt,
        capct,
        shift_mag;

} table_t;

alloc_recl_sign_templs(table);

#define sub_table_length(self)     ((unsigned short)1 << (self)->len_log2)
#define sub_table_entries_id(self) ((self)->len_log2)


#define sub_table_hash hash_univ_pow2

static const struct {_t(((table_t){}).len_log2) initial_len_log2; } sub_table = {2};


static_inline table_t *table_build_2(const entry_t *const entry_a, const entry_t *const entry_b) {
    table_t *const self = table_alloc();

    self->len_log2  = sub_table.initial_len_log2;
    self->shift_mag = bit_sz(self->coef) - sub_table.initial_len_log2;

    self->slots     = entries_pow2_new(sub_table.initial_len_log2);

    self->cnt       = 2;
    self->capct     = 2;
    self->coef      = hash_rand_coef(self->coef);

    for ( ;
        sub_table_hash(entry_a->key, self->coef, self->shift_mag) == sub_table_hash(entry_b->key, self->coef, self->shift_mag) ;
        self->coef = hash_rand_coef(self->coef)
    ) ;

    self->slots[sub_table_hash(entry_a->key, self->coef, self->shift_mag)] = (entry_t *)entry_a;
    self->slots[sub_table_hash(entry_b->key, self->coef, self->shift_mag)] = (entry_t *)entry_b;

    return self;
}

static_inline void sub_table_cleand_recl(table_t *const self) {
    entries_pow2_recl_cleand(self->slots, sub_table_entries_id(self));
    table_recl(self);
}

static_inline _t(((entry_t){}).item) sub_table_query(const table_t *const self, const _t(((entry_t){}).key) key) {
    return entry_query(self->slots[sub_table_hash(key, self->coef, self->shift_mag)], key);
}


static_inline void sub_table_clean_prts_recl(table_t *const self) {
    unsigned short curr;
    for (curr = 0; self->cnt--; curr++) {
        for (; self->slots[curr] == empty_entry; curr++) ;

        entry_recl(self->slots[curr]);
        self->slots[curr] = (void *)empty_entry;
    }
    sub_table_cleand_recl(self);
}


static_inline void sub_table_rebuild(table_t *const self, const entry_t *src[]) {
    fld_t *const fld  = fld_pow2_new(self->len_log2);
    unsigned short id, hashes[self->cnt];
    _t(self->cnt) cnt;
    for (cnt = 0; cnt < self->cnt; cnt++) {
        id = hashes[cnt] = sub_table_hash(src[cnt]->key, self->coef, self->shift_mag);

        if (fld_get(fld, id) == 0)
            fld_flip(fld, id);
        else
            for (self->coef = hash_rand_coef(self->coef); cnt--; )
                fld->words[hashes[cnt] / bit_sz(fld->words[0])] = 0;
    }

#   define setentry(index) ({                                       \
        self->slots[(id = hashes[index])] = (entry_t *)src[index];  \
        fld->words[id / bit_sz(fld->words[0])] = 0;                 \
    })
    cnt = self->cnt % 4;
    switch (cnt) {
        case 3: setentry(2);
        case 2: setentry(1);
        case 1: setentry(0);
        case 0: for (; cnt < self->cnt; cnt += 4) {
                setentry(cnt);
                setentry(cnt + 1);
                setentry(cnt + 2);
                setentry(cnt + 3);
            }
    }
#   undef setentry
    fld_pow2_recl_clnd(fld, self->len_log2);
}

static_inline void sub_table_entrs(register table_t *const self, const entry_t *dest[]) {
    unsigned char  cnt;
    unsigned short curr;

    for ((cnt = 0), (curr = 0); cnt < self->cnt; cnt++, curr++) {
        for (; self->slots[curr] == empty_entry; curr++);

        dest[cnt]         = self->slots[curr];
        self->slots[curr] = (void *)empty_entry;
    }
}

static_inline void sub_table_expand(register table_t *const self, const entry_t *const append) {
    const entry_t *entries[self->cnt + 1];

    sub_table_entrs(self, entries);
    entries[self->cnt++] = append;

    entries_pow2_recl_cleand(self->slots, sub_table_entries_id(self));

    self->capct      *= 2;
    self->shift_mag  -= 2;
    self->len_log2   += 2;
    self->slots       = entries_pow2_new(sub_table_entries_id(self));

    sub_table_rebuild(self, entries);
}


static_inline void sub_table_rehash(register table_t *const self, const entry_t *const append) {
    const entry_t *entries[self->cnt + 1];

    sub_table_entrs(self, entries);
    entries[self->cnt++] = append;

    self->coef = hash_rand_coef(self->coef);
    sub_table_rebuild(self, entries);
}

static_inline void sub_table_set_entry(
    register table_t *const self,
    const unsigned short    id,
    const entry_t *const    entry
) {
    self->slots[id] = (void *)((self->slots[id] == empty_entry) ? entry : self->slots[id]);

    self->cnt += (self->slots[id] == entry);

    if (self->slots[id] != entry) {
        if (self->cnt >= self->capct)  // <<<< only expand on collisions ....
            sub_table_expand(self, entry);
        else
            sub_table_rehash(self, entry);
    }
}
#endif

