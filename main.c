//
//  main.c
//  dyn_perf
//
//  Created by Samy Vilar on 9/28/14.
//  Copyright (c) 2014 samyvilar. All rights reserved.
//

#include <stdio.h>

#include "comp_utils.h"
#include "mt_rand.h"
#include "dyn_perf.h"


int key_diff(const void *a, const void *b)
{   return ((*(entry_t **)a)->key < (*(entry_t **)b)->key) ? -1 : 1;    }

void set_random_keys_and_values(
    _t(entry_null->key)  *keys,
    _t(entry_null->item) *values,
    const unsigned long cnt
) {
    typedef _t(*keys) key_t;

    assert(cnt <= (key_t)-1);

    entry_t
        *const entries         = malloc(_s(*entries) * cnt),
        **const sorted_entries = malloc(_s(entries)  * cnt);
    size_t index;

    static unsigned long (*const rand)() = (void *)&rand_64;
    for (index = 0; index < cnt; index++) {
        entries[index] = (entry_t){.key = rand(), .item = (_t(*values))rand()};
//        entries[index] = (entry_t){.key = index, .item = (_t(*values))rand()};
        sorted_entries[index] = &entries[index];
    }

    qsort(sorted_entries, cnt, _s(*sorted_entries), key_diff);

    entry_t **const duplcts = malloc(_s(entries) * cnt);
    size_t curr, duplct_cnt = 0, range_cnt = 1;

    struct {key_t low, high;} *const avlbl_ranges = malloc((cnt + 2) * _s(*avlbl_ranges));

    if (0 != sorted_entries[0]->key)
        avlbl_ranges[0] = (_t(*avlbl_ranges)){.low = 0, .high = sorted_entries[0]->key};
    else
        avlbl_ranges[0].low = avlbl_ranges[0].high = 0;

    for (index = 0; index < cnt; index = curr) {
        for (curr = index + 1; curr < cnt && sorted_entries[index]->key == sorted_entries[curr]->key; curr++)
            duplcts[duplct_cnt++] = sorted_entries[curr];

        if (sorted_entries[index]->key != (key_t)-1) {
            avlbl_ranges[range_cnt].low = sorted_entries[index]->key + 1;
            avlbl_ranges[range_cnt].high = curr < cnt ? sorted_entries[curr]->key : (key_t)-1;

            range_cnt += avlbl_ranges[range_cnt].low != avlbl_ranges[range_cnt].high;
        }
    }

    if (sorted_entries[cnt - 1]->key != (key_t)-1) {
        avlbl_ranges[range_cnt].low = sorted_entries[cnt - 1]->key + 1;
        avlbl_ranges[range_cnt++].high = 0;
    }

    for (index = 0; duplct_cnt && index < range_cnt; index++)
        while (duplct_cnt && avlbl_ranges[index].low != avlbl_ranges[index].high)
            duplcts[--duplct_cnt]->key = avlbl_ranges[index].low++;

    assert(duplct_cnt == 0);

    qsort(sorted_entries, cnt, _s(*sorted_entries), key_diff);
    for (index = 0; index < (cnt - 1); index++)
        assert(sorted_entries[index]->key != sorted_entries[index + 1]->key);

    printf(
        "range: %'llu - %'llu\n"
        ,(unsigned long long)sorted_entries[0]->key
        ,(unsigned long long)sorted_entries[cnt - 1]->key
    );

    for (index = 0; index < cnt; index++) {
        keys[index] = entries[index].key;
        values[index] = entries[index].item;
    }

    free(entries);
    free(sorted_entries);
    free(duplcts);
    free(avlbl_ranges);
}


int main()
{
    typedef _t(entry_null->key)   key_t;
    typedef _t(entry_null->item)  item_t;

    const size_t test_size = umin(10000000, (key_t)-1UL);

    key_t  *keys  = malloc(test_size * _s(*keys));
    item_t *items = malloc(test_size * _s(*items));

    set_random_keys_and_values(keys, items, test_size);

    dyn_perf_t *perf = dyn_perf_new();
    test_dyn_perf(perf, keys, items, test_size);

    free(keys);
    free(items);

    fld_pow2_recl_clnd(perf->entry_type, perf->len_log2);
    entries_pow2_recl_cleand((entry_t **)perf->slots, perf->len_log2);

    dyn_perf_recl(perf);
    perf = NULL;

    // Release all allocation blocks ....
    entries_release_alloc_blocks();
    fld_pow2_release_alloc_blocks();

    entry_release_alloc_blocks();
    table_release_alloc_blocks();
    dyn_perf_release_alloc_blocks();

    return 0;
}
