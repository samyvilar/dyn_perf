
#include "alloc.h"
#include "dyn_perf.h"

typedef vect_lrgst_intgl_type pckd_t;

typedef _t(*dyn_perf_null->slots->table) table_t;

alloc_rec_templs(table);

#define table_null ((table_t *)NULL)

static const struct {
    const _t(table_null->len_log2)
        initial_len_log2,
        initial_irrlvnt_bits;
} sub_table = {
#   define SUB_TABLE_INITL_LEN_LOG2 2

    .initial_len_log2       = SUB_TABLE_INITL_LEN_LOG2,
    .initial_irrlvnt_bits   = (bit_sz(entry_null->key) - SUB_TABLE_INITL_LEN_LOG2),

#   define capct_from_len_log2(len) \
        comp_select(len == 2,  2,   \
        comp_select(len == 4,  4,   \
        comp_select(len == 6,  8,   \
        comp_select(len == 8,  16,  \
        comp_select(len == 10, 32,  \
        comp_select(len == 12, 64,  \
            (void)0))))))

#   undef capct_from_len_log2
#   undef SUB_TABLE_INITL_LEN_LOG2
};

table_t *table_build_2(entry_t *a, entry_t *b) {
    typedef _t(a->key) memb_t;

    memb_t coef, hash_a, hash_b;
    const memb_t key_a = a->next->key, key_b = b->next->key;
    do {
        coef = hash_rand_coef(coef);
        hash_a = hash_univ_pow2(key_a, coef, sub_table.initial_irrlvnt_bits);
        hash_b = hash_univ_pow2(key_b, coef, sub_table.initial_irrlvnt_bits);
    } while (hash_a == hash_b) ;

    table_t *self       = table_alloc();
    self->cnt           = 2;
    self->len_log2      = sub_table.initial_len_log2;
    self->coef          = coef;
    self->slots         = entries_pow2_new(sub_table.initial_len_log2);
    self->slots[hash_a] = a;
    self->slots[hash_b] = b;

    return self;
}

void sub_table_rehash(table_t *self, entry_t **curr, entry_t *append, _t(table_null->len_log2) id)
{
    entry_t *entries[self->cnt];
    entries[0] = append;
    entries[1] = *curr;
    *curr      = (void *)empty_entry;

    _t(self->cnt) cnt = self->cnt;
    for (curr = self->slots - 1; cnt-- > 2; entries[cnt] = *curr)
        while (*++curr == empty_entry) ;

    entries_pow2_recl_cleand(entries_pow2_init(self->slots, id), id);
    self->slots = entries_pow2_new(self->len_log2);

    typedef _t(entry_null->key) key_t;
    typedef vect_lrgst_intgl_type packd_t;

    _t(vect.lrgst.intgl.ops->load)                  load  = vect.lrgst.intgl.ops->load_align;
    _t(vect.lrgst.intgl.ops->store)                 store = vect.lrgst.intgl.ops->store_align;
    _t(vect.lrgst.intgl.ops->mul[_s(key_t)])        mul   = vect.lrgst.intgl.ops->mul[_s(key_t)];
    _t(vect.lrgst.intgl.ops->rshft_lgcl[_s(key_t)]) rshft = vect.lrgst.intgl.ops->rshft_lgcl[_s(key_t)];

    packd_t (*const brdcst)(key_t)       = vect.lrgst.intgl.ops->brdcst[_s(key_t)];
    packd_t (*const set_shft)(long long) = vect.lrgst.intgl.ops->brdcst[_s(long long)];

    const size_t remndr = self->cnt % (_s(pckd_t)/_s(key_t));
    size_t key_cnt = self->cnt + (remndr ? ((_s(pckd_t)/_s(key_t)) - remndr) : 0);

    packd_t packd_keys[key_cnt/(_s(packd_t)/_s(key_t))] __attribute__((aligned(_s(pckd_t))));
    _t(packd_keys) packd_ids __attribute__((aligned(_s(pckd_t))));

    fld_t set[fld_len(self->len_log2)];
    const pckd_t irrlvn_bits = set_shft(shft_mag(self));

    for (cnt = 0; cnt < self->cnt; cnt++)
        ((key_t *)packd_keys)[cnt] = entries[cnt]->next->key;

    for (cnt = 0; cnt < self->cnt; cnt++) {
        const packd_t coef = brdcst(self->coef = hash_rand_coef(table_null->coef));

        for (id = 0; id < array_cnt(packd_keys); id++)
            store(&packd_ids[id], rshft(mul(packd_keys[id], coef), irrlvn_bits));

        for (id = 0; id < array_cnt(set); set[id++].word = 0) ;
        for (cnt = self->cnt; cnt--; fld_flip(set, ((key_t *)packd_ids)[cnt])) ;
        while (id--)
            cnt += bits_cnt_ones(set[id].word);
    }

    while (cnt--)
        self->slots[((key_t *)packd_ids)[cnt]] = entries[cnt];
}
