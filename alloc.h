

#ifndef __ALLOC__
#define __ALLOC__

#include <stdlib.h>
#include <unistd.h>

#include "comp_utils.h"


#ifndef t_frm_prefx
#   define t_frm_prefx(id) id ## _t // <<<< returns the type name from an objects prefix (assuming it was defined as typedef type_info name ## _t;)
#endif

#define alloc_mgt_t(id) id ## _alloc_mgt_t

typedef void *(*const alloctr_t)(size_t);
typedef void (*const de_alloctr_t)(void *);

#define obj_alloctr(id) id ## _alloctr_t
#define obj_reclr(id)   id ## _reclr_t

#define alloc_obj_tdef_tmpl(id)                        \
comp_time_assrt(_s(t_frm_prefx(id)) >= _s(void *));    \
typedef struct alloc_mgt_t(id){                        \
    t_frm_prefx(id)                                    \
        *recld                                         \
       ,*block                                         \
       ,*curr                                          \
    ;                                                  \
    size_t cnt;\
    const alloctr_t     alloc;                         \
    const de_alloctr_t  releas;                        \
} alloc_mgt_t(id);

#define alloc_mgt(id) _ ## id ## _alloc_mgt

#define alloc_mgt_obj(id) static alloc_mgt_t(id) alloc_mgt(id) = {.alloc = &malloc, .releas = &free}

#define reclr(id) id ## _recl

#define recl_sign_tmpl(id) void reclr(id)(t_frm_prefx(id) *const) // <<<<<<< gets signature of recycler of a type name

#define recl_tmpl(id)                               \
    inline void reclr(id)(t_frm_prefx(id) *self) {  \
        *(void **)self = alloc_mgt(id).recld;       \
        alloc_mgt(id).recld  = self;                \
    }
// ^^^^^^^^^^^^^^^^^ template for recycling objects,

#define release_alloc_blocks(id) id ## _release_alloc_blocks
#define release_alloc_blocks_sign(id) void release_alloc_blocks(id) ()

#define release_tmpl(id)                                                                \
    release_alloc_blocks_sign(id) {                                                     \
        static _t(alloc_mgt(id)) *const mgt = &alloc_mgt(id);                           \
                                                                                        \
        void **curr, *next_block = NULL;                                                \
        for (curr = (void *)mgt->block; curr; curr = (void *)next_block) {              \
            next_block = *curr;                                                         \
            mgt->releas(curr);                                                          \
        }                                                                               \
        mgt->recld = NULL; mgt->block = NULL; mgt->curr = NULL; mgt->cnt = 0;           \
    }
// ^^^^^^ Frees all blocks assuming, the caller has recycled all alocated nodes ....


// OBJECT allocator ************************************************************************************************
#define page_size() 4096LLU
#define alloc_templ(id)                                                     \
    alloc_sign_tmpl(id) {                                                   \
        static _t(alloc_mgt(id)) *mgt = &alloc_mgt(id);                     \
        static const _t(page_size()) size = page_size();                    \
                                                                            \
        if (mgt->recld) {                                                   \
            void *temp = mgt->recld;                                        \
            mgt->recld = *(void **)temp;                                    \
            return temp;                                                    \
        }                                                                   \
                                                                            \
        if (!mgt->cnt) {                                                    \
            void *new_block = mgt->alloc(size);                             \
            *(void **)new_block = mgt->block;                               \
            mgt->block = new_block;                                         \
            mgt->curr = new_block + _s(void *);                             \
            mgt->cnt = (size - _s(void *))/_s(mgt->curr[0]);                \
        }                                                                   \
        return &mgt->curr[--mgt->cnt];                                      \
    }


#define allocr(id)          id ## _alloc
#define alloc_sign_tmpl(id) t_frm_prefx(id) *allocr(id)() // <<<<< gets singature of an object allocator from its id

#define alloc_rec_templs(id)            \
    alloc_obj_tdef_tmpl(id);            \
    alloc_mgt_obj(id);                  \
    recl_tmpl(id)                       \
    alloc_templ(id)                     \
    release_tmpl(id)

#define alloc_recl_sign_templs(id)      \
    recl_sign_tmpl(id);                 \
    alloc_sign_tmpl(id);                \
    release_alloc_blocks_sign(id)

#endif
