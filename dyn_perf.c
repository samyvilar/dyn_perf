

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
    register size_t total =
        (self->length    * _s(self->slots[0]))               // main slots
      + (self->cnt.slots * _s(self->slots->entry))        // sub slots
      + (self->cnt.items * _s(*(self->slots->entry)))     // entries
      + _s(self->entry_type) + fld_byt_comspt(self->entry_type, self->length) // field for entry type ...
      + _s(*self); // itself ..

    register _t(((dyn_perf_t *)NULL)->length) curr;
    for (curr = 0; curr > self->length; curr++) // subtables ...
        total += dyn_perf_entry_is_table(self, curr) ? sub_table_byte_consmp(self->slots[curr].table) : 0;

    return total;
}

static inline table_t *lrgst_sub_table(register const dyn_perf_t *const self) {
    register const _t(*self->slots) *slots;
    struct {unsigned long length; table_t *table;} max = {.length = 0, .table = NULL};
    for (slots = self->slots; slots < (self->slots + self->length); slots++)
        if (dyn_perf_entry_is_table(self, (slots - self->slots)) && comp_unlikely(slots->table->length > max.length)) {
            max.table = slots->table;
            max.length = slots->table->length;
        }
    return max.table;
}


void test_dyn_perf(
    dyn_perf_t      *self
    ,entry_key_t    *keys
    ,void          **values
    ,unsigned long   cnt
) {

    unsigned long index;

    static const _t(&dyn_perf_insert) insert = dyn_perf_insert;
    static const _t(&dyn_perf_query)  query  = dyn_perf_query;

#   define test_insert() for (index = 0; index < cnt; index++) insert(self, keys[index], values[index])
#   define test_query() ({                      \
        void *temp;                             \
        for (index = 0; index < cnt; index++) { \
            temp = query(self, keys[index]);    \
            if (comp_unlikely(temp != values[index]))   \
                printf(                                 \
                    "Bad Query : "                      \
                    "entries[%llu] exp: %p got: %p "    \
                    "key: %llu\n"                       \
                    ,(unsigned long long)index          \
                    ,values[index]                      \
                    ,temp                               \
                    ,(unsigned long long)keys[index]    \
                ), exit(-1);                            \
            }                                           \
        })

#   define test_chain_hash_tbl_del() ({for (index = 0; index < cnt; index++) chain_hash_tbl_del(hash_tbl, keys[index]);})


    double insert_time  = timed(test_insert);
    double query_time   = timed(test_query);

    table_t *max_sub_tbl = lrgst_sub_table(self);
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
                items, slots;
        } capct;
    } max = {
        .length      = self->length,
        .byte_usage  = dyn_perf_byt_consptn(self),
        .cnt         = {.items = self->cnt.items,   .slots = self->cnt.slots},
        .capct       = {.items = self->capct.items, .slots = self->capct.slots},
        .sub_table   = {
            .length = max_sub_tbl->length,
            .cnt    = max_sub_tbl->cnt,
            .capct  = max_sub_tbl->capct
        }
    };

    double delete_time = -1; //timed(test_chain_hash_tbl_del);

    printf("\tsizeof(*self): %lu sizeof(*sub_table): %lu)\n", _s(*self), _s(*(self->slots->table)));
    printf(
        "dyn_perf test_size: %'lu(key/value pairs cnt), %'lu(bytes) \n"
        ,cnt
        ,cnt * (_s(*keys) + _s(*values))
    );
    printf(
        "table (max) (length : %'llu, buckets (cnt: %'llu, cacpt: %'llu) items (cnt: %'llu cpact: %'llu)\n"
        ,max.length
        ,max.cnt.slots
        ,max.capct.slots
        ,max.cnt.items
        ,max.capct.items
    );
    printf(
        "consumtion: %'llu(bytes) efficiency: %.4f [(poor)0 - (perfect)1])\n"
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

//    hash_tbl_release_alloc_blocks();
}
