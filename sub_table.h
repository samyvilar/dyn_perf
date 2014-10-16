

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

    _t(((entry_t){}).key)
        coef;

    union {
        struct {
            unsigned char
                irrlvnt_bits,
                len_log2,
                cnt,
                capct;
        };

        unsigned
            stats;
    };
} table_t;

alloc_recl_sign_templs(table);


#define sub_table_length(self)     (1 << (self)->len_log2)
#define sub_table_entries_id(self) ((self)->len_log2)

#define sub_table_rand_coef(type)


static_inline _t(((entry_t){}).key) sub_table_hash(const table_t *const self, const _t(((entry_t){}).key) key) {
    return hash_univ_pow2(key, self->coef, self->irrlvnt_bits);
}

table_t *table_build_2(entry_t *entry_a, entry_t *entry_b);

_t(((entry_t){}).item) sub_table_query(const table_t *const self, const _t(((entry_t){}).key) key);

static_inline void sub_table_cleand_recl(table_t *const self) {
    entries_pow2_recl_cleand(self->slots, self->len_log2);
    table_recl(self);
}


static_inline void entrs_coll_clr(entry_t **src, entry_t **dest, size_t cnt) {
    _t(cnt) curr;
    for (curr = 0; cnt; src[curr++] = (void *)empty_entry) {
        for (; empty_entry == src[curr]; curr++) ;
        dest[--cnt] = src[curr];
    }
}



static_inline void sub_table_rehash(table_t *self, entry_t **curr, entry_t *append, unsigned short id) {
    typedef lrgst_vect_ingtl_t      oprn_t;
    typedef _t(((entry_t){}).key)   memb_t;

    entry_t *entries[self->cnt];
    entries[0] = append;
    entries[1] = *curr;
    *curr      = (void *)empty_entry;
    entrs_coll_clr(self->slots, &entries[2], self->cnt - 2);
    entries_pow2_recl_cleand(self->slots, id);

    memb_t keys[self->cnt] __attribute__((aligned(sizeof(oprn_t))));
    for (id = 0; id < self->cnt; id++)
        keys[id] = entries[id]->key;


    oprn_t ids;
    unsigned char cnt;
    const _t(cnt) termnl = self->cnt - (self->cnt % (_s(oprn_t)/_s(memb_t)));

    hashr_t *hash_buff = hashr_init(&(hashr_t){}, self->coef = hash_rand_coef(self->coef), self->irrlvnt_bits);
    fld_t *const set = fld_pow2_new(self->len_log2);

    memb_t (*const get)(oprn_t, const int) = (_t(get))vect.lrgst.intgl.ops->get[_s(memb_t)];

    restart:
    for (cnt = 0 ; cnt < termnl; cnt += _s(oprn_t)/_s(memb_t)) {
        ids = hashes(hash_buff, &keys[cnt]);

        for (id = 0; id < _s(oprn_t)/_s(memb_t); id++) {
            unsigned short curr = get(ids, id);
            fld_flip(set, curr);
            if (fld_get(set, curr))
                continue;
            fld_clr(set, self->len_log2);
            hashr_init_coef(hash_buff, self->coef = hash_rand_coef(self->coef));
            goto restart;
        }
    }
    for ( ; cnt < self->cnt; cnt++) {
        id = sub_table_hash(self, keys[cnt]);
        fld_flip(set, id);
        if (fld_get(set, id))
            continue ;
        fld_clr(set, self->len_log2);
        hashr_init_coef(hash_buff, self->coef = hash_rand_coef(self->coef));
        goto restart;
    }

    self->slots = entries_pow2_new(self->len_log2);
    for (cnt = 0; cnt < termnl; cnt += _s(oprn_t)/_s(memb_t))
        for ((ids = hashes(hash_buff, &keys[cnt])), (id = 0); id < _s(oprn_t)/_s(memb_t); id++)
            self->slots[get(ids, id)] = entries[id + cnt];

    for ( ; cnt < self->cnt; cnt++)
        self->slots[sub_table_hash(self, keys[cnt])] = entries[cnt];
}



#endif

