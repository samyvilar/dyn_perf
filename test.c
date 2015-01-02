
#include <stdio.h>
#include <string.h>
#include <locale.h>

#include "dyn_perf.h"

#include "timed.h"

typedef _t(*dyn_perf_null->slots->table) table_t;

static table_t lrgst_sub_table(dyn_perf_t *self) {
    table_t max = {.len_log2 = 0};

    size_t curr = length(self);
    while (curr--)
        if (dyn_perf_entry_is_table(self, curr) && (self->slots[curr].table->len_log2 > max.len_log2))
            max = *self->slots[curr].table;

    max.len_log2 = max.len_log2 ? max.len_log2 : 64; // <<<< if length is zero then set to 64 so (1 << 64) == 0
    return max;
}

typedef struct {size_t cnt, slots, entries, items[128], freqncs[128];} sub_tbl_cnts_t;

static sub_tbl_cnts_t *compl_sub_table_stats(const dyn_perf_t *const self, sub_tbl_cnts_t *stats)
{
    _t(self->entry_type->word) set;
    size_t len = fld_len(self->len_log2);
    _t(dyn_perf_null->slots) block = &self->slots[len << log2_frm_pow2[bit_sz(set)]];

    while (len--) {
        block -= bit_sz(set);
        for (set = self->entry_type[len].word; 0 != set; set &= set - (_t(set))1) {
            stats->cnt++;

            _t(self->slots->table) table = block[bits_trlng_zrs(set)].table;

            stats->slots += length(table);
            stats->entries += table->cnt;

            stats->items[sub_table_capct(table)] += table->cnt;
            stats->freqncs[sub_table_capct(table)]++;
        }
    }
    return stats;
}

void test_dyn_perf(dyn_perf_t *const self, _t(entry_null->key) *const keys, _t(entry_null->item) *const values, const size_t cnt) {
    typedef _t(*keys)   key_t;
    typedef _t(*values) item_t;

    size_t index;
    const _t(*self) orignl = *self;
    static const struct {
        const _t(&dyn_perf_setitem) set;
        const _t(&dyn_perf_getitem) get;
        const _t(&dyn_perf_delitem) rem;
    } oprtr = {.set = dyn_perf_setitem, .get = dyn_perf_getitem, .rem = dyn_perf_delitem};


#   define test_insert() for (index = 0; index < cnt; index++) oprtr.set(self, keys[index], values[index]);

#   define test_query() ({                                  \
        item_t temp;                                        \
        for (index = 0; index < cnt; index++)               \
            if ((temp = oprtr.get(self, keys[index])) != values[index])   \
                printf(                                     \
                    "Bad Query : "                          \
                    "entries[%'llu] exp: %p got: %p "        \
                    "key: %llu\n"                           \
                    ,(unsigned long long)index              \
                    ,(void *)(unsigned long)values[index]   \
                    ,(void *)(unsigned long)temp            \
                    ,(unsigned long long)keys[index]        \
                ), exit(-1);                                \
        })

#   define test_del() for (index = 0; index < cnt; index++) oprtr.rem(self, keys[index])

    double insert_time  = timed(test_insert);
    double query_time   = timed(test_query);

    size_t curr = 0, max_sqntl_empty_slots = 0, max_sqntl_non_empty_slots = 0;
    for (index = 0; index < length(self); index++)
        if (self->slots[index].entry == empty_entry)
            curr++;
        else if (curr > max_sqntl_empty_slots) {
            max_sqntl_empty_slots = curr;
            curr = 0;
        } else
            curr = 0;
    printf("max_sqntl_empty_slots: %zu\n", max_sqntl_empty_slots);

    for (index = 0; index < length(self); index++)
        if (self->slots[index].entry != empty_entry)
            curr++;
        else if (curr > max_sqntl_non_empty_slots) {
            max_sqntl_non_empty_slots = curr;
            curr = 0;
        } else
            curr = 0;
    printf("max_sqntl_non_empty_slots: %zu\n", max_sqntl_non_empty_slots);



    table_t max_sub_tbl = lrgst_sub_table(self);
    sub_tbl_cnts_t *sub_table_stats = compl_sub_table_stats(self, memset(&(sub_tbl_cnts_t){}, 0, _s(sub_tbl_cnts_t)));

    const struct {
        size_t length, byte_usage, cnt;
        struct {size_t length, cnt, capct;} sub_table;
        struct {size_t items, slots, min;} capct;
    } max = {
        .length      = length(self),
        .byte_usage  = dyn_perf_byt_consptn(self),
        .cnt         = self->cnt,
        .capct       = {.items = dyn_perf_capct(self), .min = dyn_perf_thrshld(self)},
        .sub_table   = {
            .length = length(&max_sub_tbl),
            .cnt    = max_sub_tbl.cnt,
            .capct  = sub_table_capct(&max_sub_tbl)
        }
    };


    size_t collsn_cnt = 0;
    for (index = 0; index < 10000000; index++) {
        const key_t colldn_key = hash_rand_coef(colldn_key);

        dyn_perf_packd_ids_t ids;
        dyn_perf_hashes(self, colldn_key, &ids);

        size_t indx;
        _t(self->slots[0]) *slot = NULL;
        for (indx = 0; indx < array_cnt(ids.memb); indx++) {
            slot = &self->slots[ids.memb[indx]];
            if (dyn_perf_entry_is_table(self, ids.memb[indx]))
                slot = (void *) &slot->table->slots[hash(slot->table, colldn_key)];
            if (slot->entry->next->key == colldn_key) {
                slot = NULL;
                break ;
            }
        }

        if (slot) {
            if (dyn_perf_getitem(self, colldn_key) != empty_entry->item) {
                printf( // ^^^^^ check getitem returns empty_entry item on key collision.
                    "test failed!, key: %llu other: %llu, item: %p other: %p\n"
                    ,(unsigned long long)colldn_key
                    ,(unsigned long long)slot->entry->next->key
                    ,slot->entry->next->item
                    ,dyn_perf_getitem(self, colldn_key)
                ), exit(-1);
            } else
                collsn_cnt++;
        }
    }


    double delete_time = timed(test_del);

    setlocale(LC_NUMERIC, "");
    for (index = cnt; index--; assert(oprtr.get(self, keys[index]) == empty_entry->item))
        ;

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
        ,(size_t)length(&orignl)
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
        "largest sub table: items (cnt %'zu, sub_table_capct %'zu) slots cnt %'zu\n"
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
                "\t\tsub_table_capct: %'zu cnt: %'zu %.2f%% items: %'zu %.4f%%\n"
                ,index
                ,sub_table_stats->freqncs[index]
                ,(sub_table_stats->freqncs[index]/(double) sub_table_stats->cnt) * 100
                ,sub_table_stats->items[index]
                ,(sub_table_stats->items[index]/(double) max.cnt) * 100
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
        "rates (items/sec) {insert %'i query rate: %'i delete rates: %'i}\n"
        ,(int)(max.cnt/insert_time)
        ,(int)(max.cnt/query_time)
        ,(int)(max.cnt/delete_time)
    );

    printf(
        "empty table final state {cnt: %'zu length: %'zu slots: %'lu consuming: %'zu (bytes)}\n\n"
        ,(size_t)self->cnt
        ,(size_t)length(self)
        ,({ size_t sum = 0; for (index = length(self); index--; sum += self->slots[index].entry != empty_entry) ;
           sum; })
        ,dyn_perf_byt_consptn(self)
    );
}
