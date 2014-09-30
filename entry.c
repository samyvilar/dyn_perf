


#include "entry.h"
#include "alloc.h"

entry_t *empty_entry = &(entry_t){.key = (entry_key_t)-1, .item = NULL};

alloc_rec_templs(entry);
