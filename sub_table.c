
#include "sub_table.h"
#include "alloc.h"
#include "dyn_perf.h"
#include "vect.h"
#include "sse2.h"

alloc_rec_templs(table);


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
            ((bit_sz(((table_t){}).coef) - SUB_TABLE_INITL_LEN_LOG2) << bit_offst(_t(table_pack_stats_t), irrlvnt_bits))
        | (SUB_TABLE_INITL_LEN_LOG2 << bit_offst(_t(table_pack_stats_t), len_log2))
        | (2 << bit_offst(_t(table_pack_stats_t), cnt))
        | (lens_log2_to_capct(SUB_TABLE_INITL_LEN_LOG2) << bit_offst(_t(table_pack_stats_t), capct))

#   undef lens_log2_to_capct
#   undef SUB_TABLE_INITL_LEN_LOG2
};


table_t *table_build_2(entry_t *entry_a, entry_t *entry_b) {
    typedef _t(entry_a->key)        memb_t;
    typedef vect_lrgst_intgl_type   oprn_t;

    table_t *const self = table_alloc();

    self->packd_stats = sub_table.table_build_2_stats;
    self->slots = entries_pow2_new(sub_table.initial_len_log2);

    hashr_t *hashr = hashr_init(&(hashr_t){}, (self->coef = hash_rand_coef(self->coef)), self->irrlvnt_bits);

    const memb_t keys[_s(oprn_t)/_s(memb_t)] __attribute__((aligned(sizeof(oprn_t)))) = {entry_a->key, entry_b->key};

    memb_t (*const get)(oprn_t, const int) = (_t(get))vect.lrgst.intgl.ops->get[_s(memb_t)];
    oprn_t buffr;
    for (buffr = hashes(hashr, keys); get(buffr, 0) == get(buffr, 1); buffr = hashes(hashr, keys))
        hashr_init_coef(hashr, self->coef = hash_rand_coef(self->coef));

    self->slots[get(buffr, 0)] = entry_a;
    self->slots[get(buffr, 1)] = entry_b;

    return self;
}
