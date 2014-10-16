#ifndef __VECT_H__
#define __VECT_H__


#include <immintrin.h>

#include "comp_utils.h"
#include "scalrs.h"

#include "sse2.h"


#define vect_lrgst_intgl_type __m128i


static const struct {
    const _t(sse) *const sse;

    const struct {
        const struct {
            _t(sse.types.intgl) type;
            const _t(sse.ops.intgl) *const ops;
        } intgl;
    } lrgst;

} vect = {
    .sse             = &sse,
    .lrgst.intgl.ops = &sse.ops.intgl
};

#endif