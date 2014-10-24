
#include "sub_table.h"
#include "alloc.h"
#include "dyn_perf.h"
#include "vect.h"
#include "sse2.h"

alloc_rec_templs(table);


typedef _t(((entry_t){}).key) key_t;


static const struct {
    const unsigned char
        initial_len_log2,
        initial_irrlvnt_bits;

    const _t(((table_t){}).packd_stats)
        table_build_2_stats;
} sub_table = {

#   define SUB_TABLE_INITL_LEN_LOG2 2

    .initial_len_log2       = SUB_TABLE_INITL_LEN_LOG2,
    .initial_irrlvnt_bits   = (bit_sz(((table_t){}).coef) - SUB_TABLE_INITL_LEN_LOG2),

#   define lens_log2_to_capct(len_log2)  \
        comp_select(len_log2 == 2,  2,   \
        comp_select(len_log2 == 4,  4,   \
        comp_select(len_log2 == 6,  8,   \
        comp_select(len_log2 == 8,  16,  \
        comp_select(len_log2 == 10, 32,  \
        comp_select(len_log2 == 12, 64,  \
            (void)0))))))

    .table_build_2_stats  =
          ((bit_sz(((table_t){}).coef) - SUB_TABLE_INITL_LEN_LOG2) << bit_offst(table_pack_stats_t, irrlvnt_bits))
        | (SUB_TABLE_INITL_LEN_LOG2 << bit_offst(table_pack_stats_t, len_log2))
        | (2 << bit_offst(table_pack_stats_t, cnt))
        | (lens_log2_to_capct(SUB_TABLE_INITL_LEN_LOG2) << bit_offst(table_pack_stats_t, capct))

#   undef lens_log2_to_capct
#   undef SUB_TABLE_INITL_LEN_LOG2
};


table_t *table_build_2(entry_t *entry_a, entry_t *entry_b) {
    typedef key_t                   memb_t;
    typedef vect_lrgst_intgl_type   oprn_t;

    table_t *const self = table_alloc();

    self->packd_stats = sub_table.table_build_2_stats;
    self->slots = entries_pow2_new(sub_table.initial_len_log2);

    oprn_t (*const load)(memb_t *)  = (_t(load))vect.lrgst.intgl.ops->load_align;
    oprn_t (*const mul) (oprn_t, oprn_t)  = vect.lrgst.intgl.ops->mul[_s(memb_t)];
    oprn_t (*const rshft)(oprn_t, const int) = vect.lrgst.intgl.ops->rshft_lgcl_imm[(_s(memb_t))];
    oprn_t (*const brcst)(memb_t) = vect.lrgst.intgl.ops->brdcst[_s(memb_t)];
    memb_t (*const get)(oprn_t, const int) = vect.lrgst.intgl.ops->get[_s(memb_t)];

    comp_time_assrt(_s(oprn_t)/_s(memb_t) > 1);

    memb_t buff[_s(oprn_t)/_s(memb_t)] __attribute__((aligned(_s(oprn_t)))) = {entry_a->key, entry_b->key};
    oprn_t ids, keys = load(buff);

    self->coef = hash_rand_coef(self->coef);
    for (ids = rshft(mul(keys, brcst(self->coef)), sub_table.initial_irrlvnt_bits); get(ids, 0) == get(ids, 1); ids = rshft(mul(keys, brcst(self->coef)), sub_table.initial_irrlvnt_bits))
        self->coef = hash_rand_coef(self->coef);

    self->slots[get(ids, 0)] = entry_a;
    self->slots[get(ids, 1)] = entry_b;

    return self;
}


static_inline void sub_table_find_coef(table_t *self, hashr_t *params, key_t src[], key_t dest[], const size_t item_cnt)
{
    _t(((fld_t *)NULL)->word) buff[fld_len(self->len_log2)]; //__attribute__((aligned(sizeof(lrgst_vect_ingtl_t))));
    fld_t *set;
    unsigned char cnt;

    restart:
    set = mem_clr_align(buff, _s(buff));
    hashes(params, (void *)src, (void *)dest, item_cnt);
    for (cnt = self->cnt; cnt--; fld_flip(set, dest[cnt])) ;

    if (fld_cnt(set, self->len_log2) == self->cnt)
        return ;

    hashr_init_coef(params, self->coef = hash_rand_coef(self->coef));
    goto restart;
}

void sub_table_rehash(table_t *self, entry_t **curr, entry_t *append, unsigned short id) {
    typedef lrgst_vect_ingtl_t      oprn_t;
    typedef key_t   memb_t;

    entry_t *entries[self->cnt];
    entries[0] = append;
    entries[1] = *curr;
    *curr      = (void *)empty_entry;
    entrs_coll_clr(self->slots, &entries[2], self->cnt - 2);
    entries_pow2_recl_cleand(self->slots, id);

    const size_t remndr = self->cnt % (_s(oprn_t)/_s(memb_t));
    const size_t key_cnt = self->cnt + (remndr ? (_s(oprn_t) - remndr) : 0);

    memb_t keys[key_cnt]; //__attribute__((aligned(sizeof(oprn_t))));
    for (id = self->cnt; id--; keys[id] = entries[id]->key) ;

    _t(keys) buff;

    sub_table_find_coef(
        self,
        hashr_init(&(hashr_t){}, self->coef = hash_rand_coef(self->coef), self->irrlvnt_bits),
        keys,
        buff,
        key_cnt/(_s(oprn_t)/_s(memb_t))
    );

    self->slots = entries_pow2_new(self->len_log2);

    for (id = self->cnt; id--; self->slots[buff[id]] = entries[id]) ;
}


