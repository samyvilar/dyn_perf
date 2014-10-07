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

int key_diff(const void *key0, const void *key1) {
    return (int)(*(_t(((entry_t){}).key) *)key0 - *(_t(((entry_t){}).key) *)key1);
}

void set_random_keys_and_values(
    _t(((entry_t){}).key)  *keys,
    _t(((entry_t){}).item) *values,
    unsigned long cnt
) {
    _t(keys) sorted_keys = malloc(sizeof(_t(*keys)) * cnt);
    size_t index;

    static _t(((entry_t){}).key) (*const rand_key)() = (void *)
        comp_select(_s(*keys) == 4, &rand,
        comp_select(_s(*keys) == 8, &rand_64,
            (void)0));

    for (index = 0; index < cnt; index++) {
        values[index] = (_t(*values))rand_64();
        keys[index] = sorted_keys[index] = rand_key();
    }

    qsort(sorted_keys, cnt, sizeof(*keys), key_diff);

    for (index = 1; index < cnt; index++)
        if (sorted_keys[index] == sorted_keys[index - 1])
            printf("duplicate keys: %lu @%lu\n", (unsigned long)(keys[index]), (unsigned long)index), exit(-1);

    printf(
        "range: %'llu - %'llu\n"
        ,(unsigned long long)sorted_keys[0]
        ,(unsigned long long)sorted_keys[cnt - 1]
    );
    free(sorted_keys);
}


int main()
{
    setlocale(LC_NUMERIC, "");
#   define TEST_SIZE 10000000

    _t(((entry_t){}).key) *keys = malloc(TEST_SIZE * sizeof(*keys));
    _t(((entry_t){}).item) *items = malloc(TEST_SIZE * sizeof(*items));

    set_random_keys_and_values(keys, items, TEST_SIZE);

    dyn_perf_t *perf = dyn_perf_new();
    test_dyn_perf(perf, keys, items, TEST_SIZE);

    dyn_perf_recl(perf);

    return 0;
}