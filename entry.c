

#include "comp_utils.h"
#include "entry.h"
#include "alloc.h"

const entry_t *const empty_entry = &(entry_t){.key = (_t(((entry_t){}).key))-1};

//alloc_rec_templs(entry);

alloc_mgt_obj(entry);
alloc_templ(entry)
release_tmpl(entry)

inline void entry_recl(entry_t *const self) {
    static entry_t **const prevs = (_t(prevs))&alloc_mgt(entry)._recld;

    register const int is_good = (self != empty_entry); // <<<< don't recycle empty_entry!

    self->_next = is_good ? *prevs : self->_next;
    *prevs      = is_good ?  self  : *prevs;
}
