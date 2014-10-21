//
//  main.c
//  dyn_perf
//
//  Created by Samy Vilar on 9/28/14.
//  Copyright (c) 2014 samyvilar. All rights reserved.
//

#include <stdio.h>
#include <locale.h>

#include "dyn_perf.h"
#include "entry.h"
#include "mt_rand.h"
#include "scalrs.h"

int key_diff(const void *a, const void *b) {
    return ((*(entry_t **)a)->key < (*(entry_t **)b)->key) ? -1 : 1;
}

void set_random_keys_and_values(
    _t(((entry_t){}).key)  *keys,
    _t(((entry_t){}).item) *values,
    const unsigned long cnt
) {
    entry_t *entries = malloc(_s(*entries) * cnt);
    entry_t **sorted_entries = malloc(_s(entries) * cnt);
    size_t index;

    static _t(((entry_t){}).key) (*const rand_key)() = (void *)
#       ifdef __INTEL_COMPILER
            &rand_64
#       else
            comp_select(_s(*keys) < 8, &rand, &rand_64)
#       endif
        ;

    for (index = cnt; index--; sorted_entries[index] = &entries[index])
        entries[index] = (entry_t){.key = rand_key(), .item = (_t(*values)) rand_64()};

    qsort(sorted_entries, cnt, _s(*sorted_entries), key_diff);

    entry_t **duplcts = malloc(_s(entries) * cnt);
    size_t curr, duplct_cnt = 0, range_cnt = 0;

    struct {_t(*keys) low, high;} *avlbl_ranges = malloc((cnt + 2)* _s(*avlbl_ranges));

    for (index = 0; index < cnt; index = curr) {
        for (curr = index + 1; curr < cnt && sorted_entries[index]->key == sorted_entries[curr]->key; curr++, duplct_cnt++)
            duplcts[duplct_cnt] = sorted_entries[curr];

        if (curr < cnt) {
            avlbl_ranges[range_cnt] = (_t(*avlbl_ranges)) {.low = sorted_entries[index]->key, .high = sorted_entries[curr]->key};
            avlbl_ranges[range_cnt++].low++;
        }
    }
    avlbl_ranges[0] = (_t(*avlbl_ranges)){.low = 0, .high = sorted_entries[0]->key};
    avlbl_ranges[range_cnt] = (_t(*avlbl_ranges)){.low = sorted_entries[cnt - 1]->key, .high = 0};
    avlbl_ranges[range_cnt].low++;
    avlbl_ranges[range_cnt++].high--;

    for (index = 0; duplct_cnt && index < range_cnt; index++) {
        while (duplct_cnt && avlbl_ranges[index].low < avlbl_ranges[index].high)
            duplcts[--duplct_cnt]->key = avlbl_ranges[index].low++;
    }

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
    setlocale(LC_NUMERIC, "");

    const size_t test_size = umin(10000000, bit_sz(((entry_t *)0)->key));

    _t(((entry_t){}).key)  *keys  = malloc(test_size * sizeof(*keys));
    _t(((entry_t){}).item) *items = malloc(test_size * sizeof(*items));

    set_random_keys_and_values(keys, items, test_size);

    dyn_perf_t *perf = dyn_perf_new();
    test_dyn_perf(perf, keys, items, test_size);

    dyn_perf_recl(perf);

    return 0;
}
