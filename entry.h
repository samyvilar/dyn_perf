

#ifndef __ENTRY_H__
#define __ENTRY_H__

#include "comp_utils.h"
#include "alloc.h"

#include "scalrs.h"

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
    entry_t *self = entry_alloc();

    self->key   = key;
    self->item  = item;

    return self;
}

static_inline _t(((entry_t){}).item) entry_query(const entry_t *const self, const _t(((entry_t){}).key) key) {
//    return (self->key == key) ? self->item : 0;
    return
        (_t(((entry_t){}).item))scalr_switch_oblvs_sign_intgl(
            key,
            ~((signed long long)((self->key - key)  | (key - self->key)) >> 63) & (unsigned long long)self->item,
            ~((signed int)((self->key - key)        | (key - self->key)) >> 31) & (unsigned)self->item,
            ~((signed short)((self->key - key)      | (key - self->key)) >> 15) & (unsigned short)self->item,
            ~((signed char)((self->key - key)       | (key - self->key)) >>  7) & (unsigned char)self->item,
            (void)0
        );
}


#endif