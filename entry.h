

#ifndef __ENTRY_H__
#define __ENTRY_H__

#include "alloc.h"

typedef unsigned long entry_key_t;

typedef struct entry_t {
    union {
        struct entry_t *_next;
        entry_key_t     key;
    };
    void               *item;
} entry_t;

alloc_recl_sign_templs(entry);

static inline entry_t *entry_new(const entry_key_t key, void *item) {
    entry_t *const self = entry_alloc();
    self->key   = key;
    self->item  = item;
    return (entry_t *)self;
}

extern entry_t *empty_entry;

#endif