

#ifndef __ENTRY_H__
#define __ENTRY_H__

#include "alloc.h"

typedef unsigned long entry_key_t;

typedef struct entry_t {
    entry_key_t          key;
    union {
        struct entry_t *_next;
        void           *item;
    };
} entry_t;

alloc_recl_sign_templs(entry);

static inline entry_t *entry_new(const entry_key_t key, void *const item) {
    entry_t *const self = entry_alloc();
    self->key   = key;
    self->item  = (void *)item;
    return (entry_t *)self;
}

extern entry_t *empty_entry;

static inline void *query_entry(entry_t *self, register entry_key_t key)
{   return (self->key == key) ? self->item : NULL;  }


#endif