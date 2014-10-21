

#ifndef __ENTRY_H__
#define __ENTRY_H__

#include "comp_utils.h"
#include "alloc.h"

#include "scalrs.h"

typedef struct entry_t {
    union {
        struct entry_t *_next;
        struct {
#           ifndef ENTRY_KEY_T
                unsigned long
#           else
                ENTRY_KEY_T
#               undef ENTRY_KEY_T
#           endif

                key;
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

static_inline _t(((entry_t){}).item) entry_query(entry_t * self, _t(((entry_t){}).key) key) {

    return (self->key == key) ? self->item : empty_entry->item;

//    return (_t(entry_query(self, key))) comp_select(
//        _s(key) != _s(self->item), ({
//            const uword_t query = key, other = self->key;
//              ~((word_t)((query - other) | (other - query)) >> (bit_sz(query) - 1)) & (uword_t)self->item;
//        }),
//        scalr_switch_oblvs_sign_intgl(
//            key,
//            ~((long long)((self->key - key) | (key - self->key)) >> (bit_sz(long long) - 1)) & (unsigned long long)self->item,
//            ~((int)((self->key - key)       | (key - self->key)) >> (bit_sz(int) - 1)) & (unsigned)self->item,
//            ~((short)((self->key - key)     | (key - self->key)) >> (bit_sz(short) - 1)) & (unsigned short)self->item,
//            ~((char)((self->key - key)      | (key - self->key)) >> (bit_sz(char) - 1)) & (unsigned char)self->item,
//            (void)0
//        ));
}


#endif