#ifndef __DYN_PERF_H__
#define __DYN_PERF_H__

#include "comp_utils.h"
#include "alloc.h"
#include "entry.h"
#include "hash.h"
#include "bits.h"
#include "fld.h"
#include "entries.h"

#include "vect.h"


typedef struct dyn_perf_t {
    union {
        entry_t *entry;
        struct {
            entry_t **slots;
            _t(entry_null->key) coef;
            _t(comp_select(_s(void *) == 8 && _s(entry_null->key) == 8, (unsigned)0, (unsigned char)0))
                len_log2, cnt;
            // ^^^^^^ if pointer and coef are both the same size 8, then use unsigned,
            //        in order to increase performance, being that the compiler will automatically pad ...
            //  otherwise unsigned char being that they are far smaller ...
        } *table;
    } *slots;

    fld_t *entry_type;

    union { // <<<@@>>> DO NOT TOUCH!!!
        entry_t __ptrs; // <<<< track head/tail of link list of entries ...
        struct {entry_t *__padding, *last, *root;};   // __padding will contain the value of last ...
    };

#   define dyn_perf_compnts_cnt 4

    _t(entry_null->key)
        compnts[dyn_perf_compnts_cnt],
        __vect_pad[
            comp_select(
                dyn_perf_compnts_cnt % (_s(vect_lrgst_intgl_type)/_s(entry_null->key)),
                (_s(vect_lrgst_intgl_type)/_s(entry_null->key))
                    - (dyn_perf_compnts_cnt % (_s(vect_lrgst_intgl_type)/_s(entry_null->key))),
                0 // <<<< no padding required ...
            )
        ];

    size_t cnt;

    unsigned len_log2;
} dyn_perf_t;

#define dyn_perf_null ((dyn_perf_t *)NULL)

typedef
    union {
        _t(dyn_perf_null->compnts)  memb;
        vect_lrgst_intgl_type       packd[
            (_s(dyn_perf_null->compnts) + _s(dyn_perf_null->__vect_pad)) / _s(vect_lrgst_intgl_type)
        ];
    } __attribute__ ((aligned(_s(vect_lrgst_intgl_type))))
    dyn_perf_packd_ids_t;


#define len_log2(self)  ((self)->len_log2)
#define length(self)    (1UL << len_log2(self))
#define shft_mag(self)  ((bit_sz(entry_null->key) - len_log2(self)))


alloc_recl_sign_templs(dyn_perf);


static const struct {
    struct {
        const double expand, shrink;
    } factors;
    const _t(dyn_perf_null->len_log2) initial_length_log2;
} dyn_perf = {
    .factors = {
        .expand = 0.6, // expand when ratio between items and length exceeds  60%
        .shrink = 0.2  // shrink when ratio between items and length subceeds 20%
    },
#define DYN_PERF_INIT_LEN_LOG2 2
    .initial_length_log2 = 2 //bit_sz(((fld_t){}).words[0])
#undef DYN_PERF_INIT_LEN_LOG2
#undef dyn_perf_stats_t
};


#define dyn_perf_capct(self)   (size_t)(length(self) * dyn_perf.factors.expand)
#define dyn_perf_thrshld(self) (size_t)(length(self) * dyn_perf.factors.shrink)

static_inline unsigned sub_table_capct(_t(*dyn_perf_null->slots->table) *self) {return 1U << (len_log2(self) >> 1);}

#define hash(self, key) hash_univ_pow2(key, (self)->coef, shft_mag(self))
#define dyn_perf_entry_is_table(self, index)     fld_get((self)->entry_type, index)


static_inline dyn_perf_t *dyn_perf_new() {
    typedef vect_lrgst_intgl_type packd_t;
    dyn_perf_t *const self = dyn_perf_alloc();

    self->cnt = 0;

    size_t index;
    for (index = 0; index < array_cnt(((dyn_perf_packd_ids_t *)0)->packd); index++)
        vect.lrgst.intgl.ops->store(&((packd_t *)self->compnts)[index], hash_rand_coef_packd(self->compnts[0]));

    self->len_log2 = dyn_perf.initial_length_log2;

    self->slots = (_t(self->slots))entries_pow2_new(dyn_perf.initial_length_log2);

    self->entry_type = fld_pow2_new(dyn_perf.initial_length_log2);

    self->last = self->root = &self->__ptrs;

    self->last->next = empty_entry;

    return self;
}


static_inline void dyn_perf_hashes(
    const dyn_perf_t *const    self,
    const _t(entry_null->key)  key,
    dyn_perf_packd_ids_t      *dest
) {
    typedef vect_lrgst_intgl_type packd_t;
    typedef _t(entry_null->key)   memb_t;

    const struct {
        packd_t (*const brdcst)(memb_t);
        packd_t (*const set_shft)(unsigned long long);

        const _t(vect.lrgst.intgl.ops->load)  load, load_align;
        const _t(vect.lrgst.intgl.ops->store) store, store_align;

        const _t(vect.lrgst.intgl.ops->mul[_s(memb_t)]) mul, rshft;
    } packd = {
        .brdcst         = vect.lrgst.intgl.ops->brdcst[_s(memb_t)],
        .set_shft       = vect.lrgst.intgl.ops->brdcst[_s(long long)],

        .load           = vect.lrgst.intgl.ops->load,
        .load_align     = vect.lrgst.intgl.ops->load_align,
        .store          = vect.lrgst.intgl.ops->store,
        .store_align    = vect.lrgst.intgl.ops->store_align,

        .mul            = vect.lrgst.intgl.ops->mul[_s(memb_t)],
        .rshft          = vect.lrgst.intgl.ops->rshft_lgcl[_s(memb_t)]
    };

    if (array_cnt(self->compnts) == 1)
        dest->memb[0] = scalr_rshift_logic_scalr((memb_t)(key * self->compnts[0]), shft_mag(self));
    else {
        size_t index;
        for (index = 0; index < array_cnt(dest->packd); index++)
            packd.store_align(
                &dest->packd[index],
                packd.rshft(
                    packd.mul(packd.load(&((packd_t *) self->compnts)[index]), packd.brdcst(key)),
                    packd.set_shft(shft_mag(self))
                )
            );
    }
}

void                    dyn_perf_setitem  (      dyn_perf_t *const, const _t(entry_null->key), const _t(entry_null->item));
int                     dyn_perf_contains (const dyn_perf_t *const, const _t(entry_null->key));
_t(entry_null->item)    dyn_perf_getitem  (const dyn_perf_t *const, const _t(entry_null->key));

void                    dyn_perf_delitem  (      dyn_perf_t *const, _t(entry_null->key));


void test_dyn_perf(dyn_perf_t *, _t(entry_null->key) *const, _t(entry_null->item) *const, const unsigned long);

size_t dyn_perf_byt_consptn(const dyn_perf_t *const);

void table_release_alloc_blocks();

#endif
