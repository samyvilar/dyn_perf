

#include "dyn_perf.h"
#include "alloc.h"
#include "comp_utils.h"
#include "entry.h"
#include "fld.h"
#include "timed.h"
#include "sub_table.h"


alloc_rec_templs(dyn_perf);

//void test() {
//    typedef struct foo_t {int *temp;} foo_t;
//    register const foo_t *const self = &(foo_t){NULL};
//    typeof(self->temp) ptr = self->temp;
//    ptr++; // <<<< calng error ptr is read only ... very strange ....
//}

static inline size_t dyn_perf_byt_consptn(register const dyn_perf_t *const self)
{
    register size_t total = 0;

    total += _s(*self);
    total += dyn_perf_length(self) * _s(_t(self->slots[0]) *);
    total += self->slots_cnt * _s(entry_t *);
    total += self->cnt * _s(entry_t);
    total += fld_byt_comspt(self->entry_type, dyn_perf_length(self));

    register _t(((entry_t){}).key) curr;

    size_t cnt = 0;
    for (curr = 0; curr < dyn_perf_length(self); curr++) // subtables ...
        for (; dyn_perf_entry_is_table(self, curr); curr++)
            cnt++;

    return total + (cnt * _s(table_t));
}

static inline table_t *lrgst_sub_table(register const dyn_perf_t *const self) {
    struct {unsigned long length; table_t *table;} max = {.length = 0, .table = NULL};

    if (self->slots_cnt == 0)
        return max.table;

    _t(self->slots->entry->key) curr, termnl = dyn_perf_length(self);
    for (curr = 0; curr < termnl; curr++) {
        for (; curr < termnl && !dyn_perf_entry_is_table(self, curr); curr++) ;

        if (curr >= termnl) break ;

        if (sub_table_length(self->slots[curr].table) > max.length) {
            max.table = self->slots[curr].table;
            max.length = sub_table_length(max.table);
        }
    }
    return max.table;
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
    static const _t(&dyn_perf_query)   query  = dyn_perf_query;
    static const _t(&dyn_perf_delitem) remove = dyn_perf_delitem;

#   define test_insert() for (index = 0; index < cnt; index++) insert(self, keys[index], values[index])
#   define test_query() ({                              \
        _t(((entry_t){}).item) temp;                    \
        for (index = 0; index < cnt; index++) {         \
            temp = query(self, keys[index]);            \
            if (comp_unlikely(temp != values[index]))   \
                printf(                                 \
                    "Bad Query : "                      \
                    "entries[%llu] exp: %p got: %p "    \
                    "key: %llu\n"                       \
                    ,(unsigned long long)index          \
                    ,(void *)(long)values[index]        \
                    ,(void *)(long)temp                 \
                    ,(unsigned long long)keys[index]    \
                ), exit(-1);                            \
            }                                           \
        })
#   define test_del() for (index = 0; index < cnt; index++) remove(self, keys[index]);


    double insert_time  = timed(test_insert);
    double query_time   = timed(test_query);

    typedef struct {
        unsigned long long tables, entries, slots;
        unsigned long long capcts[16];
    } sub_tbl_cnts_t;

#   define cnt_sub_tables(self) ({                                  \
        register size_t index;                                      \
        sub_tbl_cnts_t cnts = {0, 0, 0, {0}};                       \
        for (index = 0; index < dyn_perf_length(self); index++)     \
            if (dyn_perf_entry_is_table(self, index))  {            \
                cnts.tables++;                                      \
                cnts.entries += self->slots[index].table->cnt;      \
                cnts.slots   += sub_table_length(self->slots[index].table);   \
                cnts.capcts[self->slots[index].table->capct]++;     \
            }                                                       \
        cnts; })

    table_t *max_sub_tbl = lrgst_sub_table(self);
    sub_tbl_cnts_t sub_table_cnts = cnt_sub_tables(self);

    const struct {
        struct {unsigned long long items, slots;} cnt;
        unsigned long long length, byte_usage;
        struct {
            unsigned long long
                length,
                cnt,
                capct;
        } sub_table;
        struct {
            unsigned long long
                items, slots, min;
        } capct;
    } max = {
        .length      = dyn_perf_length(self),
        .byte_usage  = dyn_perf_byt_consptn(self),
        .cnt         = {.items = self->cnt,   .slots = self->slots_cnt},
        .capct       = {.items = dyn_perf_capct(self), .min = dyn_perf_thrshld(self), .slots = dyn_perf_slots_capct(self)},
        .sub_table   = {
            .length = sub_table_length(max_sub_tbl),
            .cnt    = max_sub_tbl->cnt,
            .capct  = max_sub_tbl->capct
        }
    };

    double delete_time = timed(test_del);

#   define expect(a, b) if ((a) != (b)) \
        printf( \
            "failure expctd: (%s)%'llu == (%s)%'llu\n"  \
            ,#a \
            ,(unsigned long long)(a)    \
            ,#b \
            ,(unsigned long long)(b)    \
        )

    expect(self->len_log2,      orignl.len_log2);
    expect(self->cnt,           orignl.cnt);
    expect(self->slots_cnt,     orignl.slots_cnt);

    for (index = 0; index < cnt; index++)
        expect(query(self, keys[index]), 0);


    printf("\n\nsizeof(*self): %lu sizeof(*sub_table): %lu) sizeof(entry_t): %lu\n", _s(*self), _s(*(self->slots->table)), _s(entry_t));
    printf(
        "Initial cnt: %'lu length: %'lu slots: %'lu \n" // "capacities: {items: %'lu slots: %'lu}\n
        ,(_t(cnt))orignl.cnt
        ,(_t(cnt))dyn_perf_length(&orignl)
        ,(_t(cnt))orignl.slots_cnt
    );
    printf(
        "dyn_perf test_size: %'lu(key/value pairs cnt), %'lu(bytes) \n"
        ,cnt
        ,cnt * (_s(*keys) + _s(*values))
    );
    printf(
        "table (max) (length : %'llu buckets (cnt: %'llu cacpt: %'llu) items (cnt: %'llu cpact: %'llu thrshld: %'llu)\n"
        ,max.length
        ,max.cnt.slots
        ,max.capct.slots
        ,max.cnt.items
        ,max.capct.items
        ,max.capct.min
    );
    printf(
        "sub tables: {cnt: %'llu entries: %'llu %.2f%% slots: %'llu %.2f%%}\n"
        ,sub_table_cnts.tables
        ,sub_table_cnts.entries
        ,(sub_table_cnts.entries/(double)max.cnt.items) * 100
        ,sub_table_cnts.slots
        ,(sub_table_cnts.slots/(double)max.length) * 100
    );
    printf("\tdistributions: \n");
    for (index = 0; index < array_cnt(sub_table_cnts.capcts); index++)
        if (sub_table_cnts.capcts[index])
            printf(
                "\t\tcapct: %'lu cnt: %'llu %.2f%%\n"
                ,index
                ,sub_table_cnts.capcts[index]
                ,(sub_table_cnts.capcts[index]/(double)sub_table_cnts.tables) * 100
            );

    printf(
        "consumtion: %'llu(bytes) efficiency: %.4f [(poor)0 - (ideal)1])\n"
        ,max.byte_usage
        ,(double)(max.cnt.items * (_s(*keys) + _s(*values))) / max.byte_usage
    );

    printf(
        "largest sub table: items (cnt %'llu, capct %'llu) slots cnt %'llu\n"
        ,max.sub_table.cnt
        ,max.sub_table.capct
        ,max.sub_table.length
    );

    printf(
        "insert_time: %.4fs query_time: %.4fs delete_time: %.4fs\n"
        ,insert_time
        ,query_time
        ,delete_time
    );
    printf(
        "final cnt: %'lu length: %'lu slots: %'lu \n" // capacities: {items: %'lu slots: %'lu}\n\n"
        ,(_t(cnt))self->cnt
        ,(_t(cnt))dyn_perf_length(self)
        ,(_t(cnt))self->slots_cnt
    );

//    hash_tbl_release_alloc_blocks();
}
