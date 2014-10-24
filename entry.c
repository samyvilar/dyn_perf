

#include "comp_utils.h"
#include "entry.h"
#include "alloc.h"

const entry_t *const empty_entry = &(entry_t){.key = (_t(((entry_t){}).key))-1};

alloc_rec_templs(entry);