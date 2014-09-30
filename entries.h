

#ifndef __ENTRIES_H__
#define __ENTRIES_H__

#include "entry.h"
#include "hash.h"
#include "bits.h"
#include "mem.h"

extern entry_t **cleand_entries[bit_size(unsigned long)];

#define entries_calc_id bits_leadn_one

static inline _t(cleand_entries[0]) entries_new(size_t length) {
    _t(cleand_entries[0]) *const bucket = &cleand_entries[entries_calc_id(length)];

    _t(*bucket) self =
        ((*bucket) != NULL)
              ? (*bucket)
              : mem_set_pattrn(malloc(sizeof(entry_t *) * length), (uword_t)empty_entry, length);

    *bucket = ((self == *bucket) ? (void *)*self : *bucket);
    *self = empty_entry;

    return self;
}

static inline void entries_recl_cleand(entry_t **self, size_t length) {
    entry_t ***const bucket = &cleand_entries[entries_calc_id(length)];
    *(entry_t ***)self = *bucket;
    *bucket = self;
}


#endif