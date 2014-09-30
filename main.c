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
    return (int)(*(entry_key_t *)key0 - *(entry_key_t *)key1);
}

void set_random_keys_and_values(
    entry_key_t  *keys,
    void        **values,
    unsigned long cnt
) {
    entry_key_t *sorted_keys = malloc(sizeof(entry_key_t) * cnt);
    size_t index;
    for (index = 0; index < cnt; index++) {
        values[index] = (void *)rand_64();
        keys[index] = sorted_keys[index] = rand_64();
    }

    qsort(sorted_keys, cnt, sizeof(entry_key_t), key_diff);
    for (index = 1; index < cnt; index++)
        if (sorted_keys[index] == sorted_keys[index - 1])
            printf("duplicate keys: %lu @%lu\n", (unsigned long)(keys[index]), (unsigned long)index), exit(-1);
    printf(
        "range: %llu - %llu\n"
        ,(unsigned long long)sorted_keys[0]
        ,(unsigned long long)sorted_keys[cnt - 1]
    );
    free(sorted_keys);
}


int main()
{
    setlocale(LC_NUMERIC, "");
#   define TEST_SIZE 10000000

    entry_key_t *keys = malloc(TEST_SIZE * sizeof(*keys));
    void **values = malloc(TEST_SIZE * sizeof(*values));

    set_random_keys_and_values(keys, values, TEST_SIZE);

    dyn_perf_t *perf = dyn_perf_new();
    test_dyn_perf(perf, keys, values, TEST_SIZE);

    dyn_perf_recl(perf);

    return 0;
}