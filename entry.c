

#include "entry.h"

entry_t empty_entry_obj = {.next = &empty_entry_obj, .key  = (_t(entry_null->key))-1, .item = NULL};
entry_t *const empty_entry = &empty_entry_obj;

alloc_rec_templs(entry);