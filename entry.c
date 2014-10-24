

#include "comp_utils.h"
#include "entry.h"
#include "alloc.h"

const entry_t *const empty_entry = &(entry_t){.key = (_t(((entry_t){}).key))-1};

alloc_obj_tdef_tmpl(entry);
alloc_mgt_obj(entry);
alloc_templ(entry)
release_tmpl(entry)

inline void entry_recl(entry_t *const self) {
//    if (self == empty_entry)
//        return ;

    *(entry_t **)self = alloc_mgt(entry).recld;
    alloc_mgt(entry).recld = self;
}
