

#ifndef __ENTRY_H__
#define __ENTRY_H__

#include "comp_utils.h"
#include "alloc.h"

typedef struct entry_t {
    union {
        struct entry_t *_next;
        struct {
            unsigned long key;
            void *item;
        };
    };
} entry_t;


extern const entry_t *const empty_entry;

alloc_recl_sign_templs(entry);

static_inline entry_t *entry_new(const _t(((entry_t){}).key) key, const _t(((entry_t){}).item) item) {
    entry_t *const self = entry_alloc();

    self->key   = key;
    self->item  = item;

    return (entry_t *)self;
}

static_inline _t(((entry_t){}).item) entry_query(
    const entry_t *const self,
    const _t(((entry_t){}).key) key) { return (self->key == key) ? self->item : 0; }


#endif