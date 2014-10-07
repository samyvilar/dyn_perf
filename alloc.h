

#ifndef __ALLOC__
#define __ALLOC__

#include <stdlib.h>
#include <unistd.h>

#include "comp_utils.h"

// the following assumes an address layout as: (0) static || heap() -> | <- stack() (2**n), where || is fixed and | can move either way...
//#define alloc_is_static(ptr) ((void *)(ptr) < sbrk(0))  // static allocation ie (global), assuming malloc doesn't use sbrk
//#define alloc_is_local(ptr) ((void *)&((char){0}) < (void *)(ptr)) // stack allocation
//#define alloc_is_dyn(ptr)  (!(alloc_is_static(ptr) || alloc_is_local(ptr))) // dynamic allocation ie non-static and non-global
// ^^^^^^^^^^^^^^^^^^^^^^^^ @@DO NOT USE!!!


#ifndef t_frm_prefx
#   define t_frm_prefx(id) id ## _t // <<<< returns the type name from an objects prefix (assuming it was defined as typedef type_info name ## _t;)
#endif

#define alloc_mgt_t(id) id ## _alloc_mgt_t

typedef void *(*const alloctr_t)(size_t);
typedef void (*const de_alloctr_t)(void *);

#define obj_alloctr(id) id ## _alloctr_t
#define obj_reclr(id)   id ## _reclr_t

#define alloc_obj_tdef_tmpl(id)                         \
typedef t_frm_prefx(id) *(*const obj_alloctr(id)) ();   \
typedef void (*const obj_reclr(id)) (t_frm_prefx(id) *);\
typedef struct alloc_mgt_t(id){                         \
    t_frm_prefx(id)                                     \
        *_recld                                         \
       ,*_block                                         \
       ,*_curr                                          \
       ,*_termnl                                        \
    ;                                                   \
    const alloctr_t     alloc;                          \
    const de_alloctr_t  de_alloc;                       \
} alloc_mgt_t(id);

#define alloc_mgt(id) _ ## id ## _alloc_mgt
#define alloc_mgt_obj(id)                       \
static const alloc_mgt_t(id) alloc_mgt(id) = {  \
     ._recld    = alloc_block_no_values         \
    ,._block    = alloc_block_no_values         \
    ,._curr     = alloc_block_no_values         \
    ,._termnl   = alloc_block_no_values         \
    ,.alloc     = &malloc                       \
    ,.de_alloc  = &free                         \
}


#define alloc_block_no_values       NULL


// OBJECT Recycler **************************************************************************************************
#define recl_rem(recl_objs) ({register typeof(recl_objs) obj = recl_objs; recl_objs = obj->_next; obj;})
//      ^^^^^^^^  removes (reuses) a previously recycled object.
//#define recl_add(recl_objs, obj) ({obj->_next = recl_objs; recl_objs = obj; (void)0;})
//      ^^^^^^^^ adds a new object to the recycled list (for later reuse by the allocator.)

#define reclr(id) id ## _recl

#define recl_sign_tmpl(id) void reclr(id)(t_frm_prefx(id) *const) // <<<<<<< gets signature of recycler of a type name

#define recl_tmpl(id)                                                   \
    inline void reclr(id)(t_frm_prefx(id) *const self) {                \
        typedef t_frm_prefx(id) obj_t;                                  \
        typedef obj_t **const mgt_attr_t;                               \
                                                                        \
        static mgt_attr_t prevs = (mgt_attr_t)&alloc_mgt(id)._recld;    \
        self->_next = *prevs;                                           \
        *prevs      = self;                                             \
    }
// ^^^^^^^^^^^^^^^^^ template for recycling objects,
// it uses the objects (_next) attribute, to create a link list of recycled objects.

#define release_alloc_blocks(id) id ## _release_alloc_blocks
#define release_alloc_blocks_sign(id) void release_alloc_blocks(id) ()

#define release_tmpl(id)                                                                \
    release_alloc_blocks_sign(id) {                                                     \
        typedef t_frm_prefx(id) obj_t;                                                  \
        typedef obj_t **const mgt_attr_t;                                               \
                                                                                        \
        static mgt_attr_t                                                               \
             blocks     = (mgt_attr_t)&alloc_mgt(id)._block                             \
            ,recld_objs = (mgt_attr_t)&alloc_mgt(id)._recld                             \
            ,objs       = (mgt_attr_t)&alloc_mgt(id)._curr                              \
            ,terminal   = (mgt_attr_t)&alloc_mgt(id)._termnl                            \
            ;                                                                           \
                                                                                        \
        static const obj_t *const empty = alloc_block_no_values;                        \
        register obj_t *curr, *next_block = (obj_t *)empty;                             \
                                                                                        \
        const unsigned long next_blk = (getpagesize()/sizeof(obj_t *)) - 1LU;           \
        for (curr = *blocks; curr != empty; curr = next_block) {                        \
            next_block = ((obj_t **)curr)[next_blk];                                    \
            alloc_mgt(id).de_alloc(curr);                                               \
        }                                                                               \
        *blocks = *recld_objs = *objs = *terminal = (obj_t *)empty;                     \
    }
// ^^^^^^ Frees all blocks assuming, the caller has recycled all alocated nodes ....


// OBJECT allocator ************************************************************************************************
#define page_size() 4096LLU
#define alloc_templ(id)                                                     \
    alloc_sign_tmpl(id) {                                                   \
        typedef t_frm_prefx(id) obj_t;                                      \
        typedef obj_t **const mgt_attr_t;                                   \
                                                                            \
        static const obj_t *const empty = alloc_block_no_values;            \
                                                                            \
        static volatile mgt_attr_t                                          \
             recld_objs    = (mgt_attr_t)&alloc_mgt(id)._recld              \
            ,objs          = (mgt_attr_t)&alloc_mgt(id)._curr               \
            ,terminal      = (mgt_attr_t)&alloc_mgt(id)._termnl             \
            ,block         = (mgt_attr_t)&alloc_mgt(id)._block              \
            ;                                                               \
                                                                            \
        if (*recld_objs != empty)                                           \
            return recl_rem(*recld_objs);                                   \
                                                                            \
        if (comp_unlikely(*objs >= *terminal)) {                            \
            const typeof(page_size()) size = page_size();                   \
            register obj_t *new_block = alloc_mgt(id).alloc(size);          \
            ((void **)new_block)[(size/sizeof(void *)) - 1LLU] = *block;    \
            *objs = *block = new_block;                                     \
            *terminal = &new_block[size/sizeof(obj_t)];                     \
        }                                                                   \
        return (*objs)++;                                                   \
    }


#define allocr(id)          id ## _alloc
#define alloc_sign_tmpl(id) t_frm_prefx(id) *allocr(id)() // <<<<< gets singature of an object allocator from its id

#define alloc_rec_templs(id)            \
    alloc_mgt_obj(id);                  \
    recl_tmpl(id)                       \
    alloc_templ(id)                     \
    release_tmpl(id)

#define alloc_recl_sign_templs(id)      \
    alloc_obj_tdef_tmpl(id);            \
    recl_sign_tmpl(id);                 \
    alloc_sign_tmpl(id);                \
    release_alloc_blocks_sign(id)

#endif
