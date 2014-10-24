

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
    entry_t
        **slots;

    _t(((entry_t){}).key)
        coef;

    union {
        table_pack_stats_t ;

        unsigned packd_stats;
    };
} table_t;

alloc_recl_sign_templs(table);


#define sub_table_length(self)     (1UL << (self)->len_log2)


static_inline _t(((entry_t){}).key) sub_table_hash(const table_t *const self, _t(((entry_t){}).key) key) {
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

void sub_table_rehash(table_t *self, entry_t **curr, entry_t *append, unsigned short id);


#endif

