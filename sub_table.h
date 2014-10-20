

#ifndef __SUB_TABLE_H__
#define __SUB_TABLE_H__

#include "entry.h"
#include "alloc.h"
#include "hash.h"
#include "entries.h"
#include "fld.h"

#define table_pack_stats_t struct {\
unsigned char                      \
    irrlvnt_bits,                  \
    len_log2,                      \
    cnt,                           \
    capct;                         \
}

typedef struct table_t {
    union {
        struct table_t *_next;
        entry_t       **slots;
    };

    _t(((entry_t){}).key)
        coef;

    union {
        table_pack_stats_t ;

        unsigned packd_stats;
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
    for (src--; cnt--; *src = (void *)empty_entry) {
        while (empty_entry == *++src) ;

        *dest++ = *src;
    }
}


static_inline void sub_table_find_coef(
    table_t            *self,
    hashr_t            *params,
    _t(((entry_t){}).key) src[],
    _t(((entry_t){}).key) dest[],
    const size_t item_cnt
) {
    _t(((fld_t *)NULL)->word) buff[fld_len(self->len_log2)] __attribute__((aligned(sizeof(lrgst_vect_ingtl_t))));
    fld_t *set;
    unsigned char cnt;

    restart:
        set = mem_clr_align(buff, _s(set));
        hashes(params, (void *)src, (void *)dest, item_cnt);
        for (cnt = self->cnt; cnt--; fld_flip(set, dest[cnt])) ;

        if (fld_cnt(set, self->len_log2) == self->cnt)
            return ;

        hashr_init_coef(params, self->coef = hash_rand_coef(self->coef));
    goto restart;
}

static_inline void small_sub_table_find_coef(
    table_t *self,
    hashr_t *params,
    _t(((entry_t){}).key) src[],
    _t(((entry_t){}).key) dest[],
    const size_t item_cnt
) {
    unsigned long set;
    unsigned char cnt;

    restart:
        hashes(params, (void *)src, (void *)dest, item_cnt);
        for ((set = 0UL), (cnt = self->cnt); cnt--; set ^= 1UL << dest[cnt]) ;

        if (bits_cnt_ones(set) == self->cnt)
            return ;

        hashr_init_coef(params, self->coef = hash_rand_coef(self->coef));
    goto restart;
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

    const size_t key_cnt = self->cnt + ((_s(oprn_t)/_s(memb_t)) - (self->cnt % (_s(oprn_t)/_s(memb_t))));
    memb_t keys[key_cnt] __attribute__((aligned(sizeof(oprn_t))));
    for (id = self->cnt; id--; keys[id] = entries[id]->key) ;

    _t(keys) buff;

    ((self->len_log2 > log2_frm_pow2[bit_sz(unsigned long)]) ? sub_table_find_coef : small_sub_table_find_coef)(
        self,
        hashr_init(&(hashr_t){}, self->coef = hash_rand_coef(self->coef), self->irrlvnt_bits),
        keys,
        buff,
        key_cnt/(_s(oprn_t)/_s(memb_t))
    );

    for ((self->slots = entries_pow2_new(self->len_log2)), (id = self->cnt); id--; self->slots[buff[id]] = entries[id]) ;
}



#endif

