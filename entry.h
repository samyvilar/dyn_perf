
#ifndef __ENTRY_H__
#define __ENTRY_H__

#include "comp_utils.h"
#include "alloc.h"

#include "scalrs.h"

#ifndef ENTRY_KEY_T
#   define ENTRY_KEY_T unsigned long
#endif

#ifndef ENTRY_ITEM_T
#   define ENTRY_ITEM_T void *
#endif

typedef struct entry_t {
    struct entry_t *next;
    ENTRY_ITEM_T item;
    ENTRY_KEY_T  key;
} entry_t;

#undef ENTRY_KEY_T
#undef ENTRY_ITEM_T


extern entry_t *const empty_entry;

#define entry_null ((entry_t *)NULL)

alloc_recl_sign_templs(entry);

static_inline entry_t *entry_new(const _t(entry_null->key) key, const _t(entry_null->item) item) {
    entry_t *self = entry_alloc();

    self->key   = key;
    self->item  = item;

    return self;
}

#endif