

#include "dyn_perf.h"
#include "alloc.h"
#include "comp_utils.h"
#include "entry.h"
#include "fld.h"
#include "timed.h"
#include "sub_table.h"
#include "vect.h"
#include "sse2.h"


alloc_rec_templs(dyn_perf);


static void _dyn_perf_set_entry(dyn_perf_t *volatile self, _t(((entry_t){}).key) id, entry_t *entry) {
    oprtns_t actions;
    entry_t **other;

    if (dyn_perf_entry_is_table(self, id)) {
        other = &self->slots[id].table->slots[sub_table_hash(self->slots[id].table, entry->key)];
        actions = acts.table;
        self = (void *)self->slots[id].table;
    } else {
        other = &(self->slots[id].entry);
        actions = acts.entry;
    }

    (*other != empty_entry ? actions.collsn : actions.empty)(self, other, entry);
}


void dyn_perf_rebuild(dyn_perf_t *self, unsigned char prev_id)
{
    typedef _t(((entry_t){}).key)       memb_t;
    typedef lrgst_vect_ingtl_t          oprn_t;

    entry_t **src = dyn_perf_cln_entrs(self, malloc(self->cnt * _s(self->slots->entry)), prev_id);

    entries_pow2_recl_cleand((entry_t **)self->slots, prev_id);
    fld_pow2_recl_clnd(self->entry_type, prev_id);

    self->entry_type = fld_pow2_new(self->len_log2);
    self->slots = (_t(self->slots))entries_pow2_new(self->len_log2);

    _t(self->cnt) curr;
    memb_t *keys;
    const size_t key_cnt = self->cnt + ((_s(oprn_t)/_s(memb_t)) - (self->cnt % (_s(oprn_t)/_s(memb_t))));
    for ((keys = malloc_align(key_cnt * _s(*keys))), (curr = self->cnt); curr--; keys[curr] = src[curr]->key) ;

    hashes(
        hashr_init(&(hashr_t){}, self->coef, self->irrlvnt_bits),
        (void *)keys,
        (void *)keys,
        key_cnt/(_s(oprn_t)/_s(memb_t))
    );

    for (curr = 0; curr < self->cnt; curr++)
        _dyn_perf_set_entry(self, keys[curr], src[curr]);

    free_align(keys);
    free(src);
}


void dyn_perf_delitem(dyn_perf_t *const self, const _t(((entry_t){}).key) key) {
    _t(key) id = dyn_perf_hash(self, key);

    _t(self->entry_type->word) is_sub_table = fld_get(self->entry_type, id);

    entry_t **other =
        is_sub_table ? &self->slots[id].table->slots[sub_table_hash(self->slots[id].table, key)]
            : &self->slots[id].entry;

    if (*other == empty_entry || (*other)->key != key)
        return ;

    entry_recl(*other);
    *other = (void *)empty_entry;

    if (is_sub_table && --(self->slots[id].table->cnt) == (_t(self->slots[id].table->cnt))1) {
        fld_flip(self->entry_type, id);
        table_t *table = self->slots[id].table;
        entrs_coll_clr(table->slots, &self->slots[id].entry, 1);
        table_recl(table);
    }

    if (--self->cnt < dyn_perf_thrshld(self) && self->len_log2 > dyn_perf.initial_length_log2) {
        self->irrlvnt_bits++;
        dyn_perf_rebuild(self, self->len_log2--);
    }
}


void dyn_perf_setitem(dyn_perf_t *const self, _t(((entry_t){}).key) id, const _t(((entry_t){}).item) item)
{
    void  *obj;
    oprtns_t *actns;
    entry_t **other, *entry = entry_new(id, item);

    id = dyn_perf_hash(self, id);

    if (fld_get(self->entry_type, id)) {
        obj   = self->slots[id].table;
        other = &self->slots[id].table->slots[sub_table_hash(self->slots[id].table, entry->key)];
        actns = &acts.table;
    } else {
        obj   = self;
        other = &self->slots[id].entry;
        actns = &acts.entry;
    }

    if (empty_entry == *other) {
        actns->empty(obj, other, entry);
        self->cnt++;
        return ;
    }

    if ((*other)->key == entry->key) {
        (*other)->item = entry->item;
        entry_recl(entry);
        return ;
    }

    actns->collsn(obj, other, entry);
    if (++self->cnt > dyn_perf_capct(self)) { // expand only on collisions ...
        self->irrlvnt_bits--;
        dyn_perf_rebuild(self, self->len_log2++);
    }
}



size_t dyn_perf_byt_consptn(dyn_perf_t *self) {
    size_t curr, cnt, lengths,
        total = _s(*self)
            + dyn_perf_length(self) * _s(_t(self->slots[0]) *)
            + self->cnt * _s(entry_t)
            + fld_byt_comspt(self->len_log2)
            ;

    for (curr = cnt = lengths = 0; curr < dyn_perf_length(self); curr++) // subtables ...
        for (; dyn_perf_entry_is_table(self, curr); cnt++)
            lengths += sub_table_length(self->slots[curr++].table);

    return total + (cnt * _s(*(self->slots->table))) + (lengths * _s(self->slots->table->slots[0]));
}

static inline table_t lrgst_sub_table(dyn_perf_t *self) {
    table_t max = {{0}};

    _t(self->slots->entry->key) curr;
    for (curr = 0; curr < dyn_perf_length(self); curr++)
        if (dyn_perf_entry_is_table(self, curr) && (sub_table_length(self->slots[curr].table) > sub_table_length(&max)))
            max = *(self->slots[curr].table);

    return max;
}

typedef struct {size_t cnt, slots, entries, items[128], freqncs[128];} sub_tbl_cnts_t;

static inline  sub_tbl_cnts_t *compl_sub_table_stats(const dyn_perf_t *const self, sub_tbl_cnts_t *stats) {

    size_t cnt = fld_cnt(self->entry_type, self->len_log2);
    size_t *indices = fld_entrs(self->entry_type, self->len_log2, malloc(_s(*indices) * cnt));

    for (stats->cnt = cnt; cnt--; ) {
        const table_t *const table = self->slots[indices[cnt]].table;

        stats->slots   += sub_table_length(table);
        stats->entries += table->cnt;

        stats->items[table->capct] += table->cnt;
        stats->freqncs[table->capct]++;
    }

    free(indices);
    return stats;
}



void test_dyn_perf(
    dyn_perf_t                *const self
    ,_t(((entry_t){}).key)    *const keys
    ,_t(((entry_t){}).item)   *const values
    ,const unsigned long   cnt
) {

    size_t index;
    const _t(*self) orignl = *self;

    static const _t(&dyn_perf_setitem) insert = dyn_perf_setitem;
    static const _t(&dyn_perf_getitem) query  = dyn_perf_getitem;
    static const _t(&dyn_perf_delitem) remove = dyn_perf_delitem;

#   define test_insert() for (index = 0; index < cnt; index++) insert(self, keys[index], values[index])

#   define test_query() ({                                  \
        _t(((entry_t){}).item) temp;                        \
        for (index = 0; index < cnt; index++)               \
            if (comp_unlikely((temp = query(self, keys[index])) != values[index]))   \
                printf(                                     \
                    "Bad Query : "                          \
                    "entries[%llu] exp: %p got: %p "        \
                    "key: %llu\n"                           \
                    ,(unsigned long long)index              \
                    ,(void *)(unsigned long)values[index]   \
                    ,(void *)(unsigned long)temp            \
                    ,(unsigned long long)keys[index]        \
                ), exit(-1);                                \
        })
#   define test_del() for (index = 0; index < cnt; index++) remove(self, keys[index])

    double insert_time  = timed(test_insert);
    double query_time   = timed(test_query);

    table_t max_sub_tbl = lrgst_sub_table(self);
    sub_tbl_cnts_t *sub_table_stats = compl_sub_table_stats(self, memset(&(sub_tbl_cnts_t){}, 0, _s(sub_tbl_cnts_t)));

    const struct {
        size_t length, byte_usage, cnt;
        struct {size_t length, cnt, capct;} sub_table;
        struct {size_t items, slots, min;} capct;
    } max = {
        .length      = dyn_perf_length(self),
        .byte_usage  = dyn_perf_byt_consptn(self),
        .cnt         = self->cnt,
        .capct       = {.items = dyn_perf_capct(self), .min = dyn_perf_thrshld(self)},
        .sub_table   = {
            .length = sub_table_length(&max_sub_tbl),
            .cnt    = max_sub_tbl.cnt,
            .capct  = max_sub_tbl.capct
        }
    };


#   define expect(a, b) if ((a) != (b)) \
        printf( \
            "failure: exp: (%s) -> %'llu, got:(%s) -> %'llu\n"  \
            ,#b                         \
            ,(unsigned long long)(b)    \
            ,#a                         \
            ,(unsigned long long)(a)    \
        ), exit(-1);

    size_t collsn_cnt = 0;
    _t(keys[0]) colldn_key;
    _t(values[0]) empty = empty_entry->item;
    for (index = 0; index < 10000000; index++) {
        colldn_key = hash_rand_coef(colldn_key);
        entry_t *entry = dyn_perf_entry(self, colldn_key);
        if (entry != empty_entry && entry->key != colldn_key) {// key collision
            if ((empty = dyn_perf_getitem(self, colldn_key)) != empty_entry->item) {
                printf(
                    "test failed!, key: %'llu other: %'llu, item: %p other: %p\n"
                    ,(unsigned long long)colldn_key
                    ,(unsigned long long)entry->key
                    ,(void *)entry->item
                    ,(void *)empty
                ), exit(-1);
            } else collsn_cnt++;
        }
    }

    double delete_time = timed(test_del);

    for (index = 0; index < cnt; index++)
        expect(query(self, keys[index]), empty_entry->item);


    printf(
        "\n\nsizeof(*self): %zu sizeof(*sub_table): %zu sizeof(entry_t): %zu, sizeof(key_t): %zu sizeof(item): %zu\n"
        ,_s(*self)
        ,_s(*(self->slots->table))
        ,_s(entry_t)
        ,_s(((entry_t){}).key)
        ,_s(((entry_t){}).item)
    );
    printf(
        "Initial cnt: %'zu length: %'zu consuming: %zu (bytes)\n"
        ,(size_t)orignl.cnt
        ,(size_t)dyn_perf_length(&orignl)
        ,dyn_perf_byt_consptn(self)
    );
    printf(
        "dyn_perf test_size: %'zu(key/value pairs cnt), %'zu(bytes) \n"
        ,cnt
        ,cnt * (_s(*keys) + _s(*values))
    );

    printf(
        "main table (full) (length : %'zu items (cnt: %'zu cpact: %'zu thrshld: %'zu)\n"
        ,max.length
        ,max.cnt
        ,max.capct.items
        ,max.capct.min
    );
    printf("tested %'zu random non-present colliding keys\n", collsn_cnt);
    printf(
        "largest sub table: items (cnt %'zu, capct %'zu) slots cnt %'zu\n"
        ,max.sub_table.cnt
        ,max.sub_table.capct
        ,max.sub_table.length
    );

    printf(
        "sub tables: {cnt: %'zu entries: %'zu %.2f%% slots: %'zu %.2f%%}\n"
        ,sub_table_stats->cnt
        ,sub_table_stats->entries
        ,(sub_table_stats->entries/(double)max.cnt) * 100
        ,sub_table_stats->slots
        ,(sub_table_stats->slots/(double)max.length) * 100
    );
    for (printf("\tdistributions: \n"), (index = 0); index < array_cnt(sub_table_stats->freqncs); index++)
        if (sub_table_stats->freqncs[index])
            printf(
                "\t\tcapct: %'zu cnt: %'zu %.2f%% items: %'zu %.4f%%\n"
                ,index
                ,sub_table_stats->freqncs[index]
                ,(sub_table_stats->freqncs[index]/(double) sub_table_stats->cnt) * 100
                ,sub_table_stats->items[index]
                ,sub_table_stats->items[index]/(double) max.cnt
            );

    printf(
        "consumtion: %'zu(bytes) efficiency: %.4f [(poor)0 - (ideal)1]\n"
        ,max.byte_usage
        ,(max.cnt * (_s(*keys) + _s(*values)))/(double)max.byte_usage
    );

    printf(
        "timing (sec): {insert: %.4f query: %.4f remove: %.4f}\n"
        ,insert_time
        ,query_time
        ,delete_time
    );
    printf(
        "rates (items/sec) {insert %'.2f query rate: %'.2f delete rates: %'.2f}\n"
        ,max.cnt/insert_time
        ,max.cnt/query_time
        ,max.cnt/delete_time
    );

    printf(
        "empty table final state {cnt: %'zu length: %'zu slots: %'lu consuming: %'zu (bytes)}\n\n"
        ,(_t(cnt))self->cnt
        ,(_t(cnt))dyn_perf_length(self)
        ,({size_t cnt = 0; for (index = 1UL << self->len_log2; index--; cnt += self->slots[index].entry != empty_entry)
        ; cnt;})
        ,dyn_perf_byt_consptn(self)
    );

//    hash_tbl_release_alloc_blocks();
}
