

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
        other = &(self->slots[id].table->slots[sub_table_hash(self->slots[id].table, entry->key)]);
        actions = acts.table;
        self = (void *)self->slots[id].table;
    } else {
        other = &(self->slots[id].entry);
        actions = acts.entry;
    }

    const _t(actions.empty) setentry = *other != empty_entry ? actions.collsn : actions.empty;
    setentry(self, other, entry);
}


void dyn_perf_rebuild(dyn_perf_t *self, unsigned char prev_id)
{
    typedef _t(((entry_t){}).key)       memb_t;
    typedef lrgst_vect_ingtl_t          oprn_t;

    entry_t
        **src = dyn_perf_cln_entrs(self, malloc(self->cnt * _s(self->slots->entry)), prev_id);

    entries_pow2_recl_cleand((entry_t **)self->slots, prev_id);
    self->slots = (_t(self->slots))entries_pow2_new(self->len_log2);

    fld_pow2_recl_clnd(self->entry_type, prev_id);
    self->entry_type = fld_pow2_new(self->len_log2);

    hashr_t *hashr = hashr_init(&(hashr_t){}, self->coef, self->irrlvnt_bits);

    _t(self->cnt) curr, compnt, termnl = self->cnt - (self->cnt % (_s(oprn_t)/_s(memb_t)));
    memb_t (*const get)(oprn_t, const int) = (_t(get))vect.lrgst.intgl.ops->get[_s(memb_t)];

    for (curr = 0 ; curr < termnl; curr += _s(oprn_t)/_s(memb_t)) {
        oprn_t ids;
        for (compnt = 0; compnt < _s(oprn_t)/_s(memb_t); compnt++)
            ((memb_t *) &ids)[compnt] = src[curr + compnt]->key;

        for (ids = hashes(hashr, (memb_t *)&ids); compnt--;
             _dyn_perf_set_entry(self, get(ids, compnt), src[curr + compnt])) ;
    }

    for ( ; curr < self->cnt; curr++)
        _dyn_perf_set_entry(self, dyn_perf_hash(self, src[curr]->key), src[curr]);

    free(src);
}



size_t dyn_perf_byt_consptn(dyn_perf_t *self) {
    size_t curr, cnt, lengths,
        total = _s(*self)
            + dyn_perf_length(self) * _s(_t(self->slots[0]) *)
            + self->cnt * _s(entry_t)
            + fld_byt_comspt(self->entry_type, self->len_log2)
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

    table_t max_sub_tbl = lrgst_sub_table(self);
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
        .cnt         = {.items = self->cnt , .slots = sub_table_cnts.slots},
        .capct       = {.items = dyn_perf_capct(self), .min = dyn_perf_thrshld(self), .slots = dyn_perf_slots_capct(self)},
        .sub_table   = {
            .length = sub_table_length(&max_sub_tbl),
            .cnt    = max_sub_tbl.cnt,
            .capct  = max_sub_tbl.capct
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

    for (index = 0; index < cnt; index++)
        expect(query(self, keys[index]), 0);


    printf("\n\nsizeof(*self): %lu sizeof(*sub_table): %lu) sizeof(entry_t): %lu\n", _s(*self), _s(*(self->slots->table)), _s(entry_t));
    printf(
        "Initial cnt: %'lu length: %'lu slots: %'lu \n" // "capacities: {items: %'lu slots: %'lu}\n
        ,(_t(cnt))orignl.cnt
        ,(_t(cnt))dyn_perf_length(&orignl)
        ,(_t(cnt))0
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
        ,(_t(cnt))0
    );

//    hash_tbl_release_alloc_blocks();
}
