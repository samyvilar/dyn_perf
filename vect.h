#ifndef __VECT_H__
#define __VECT_H__


#include <immintrin.h>

#include "comp_utils.h"

#include "simd2.h"
#include "sse2.h"
#include "avx2.h"

#if defined(__AVX2__)
#   define vect_lrgst_singl_type    __m256
#   define vect_lrgst_doubl_type    __m256d

#   define vect_lrgst_intgl_type    __m256i
#   define vect_lrgst_intgl_oprts   &avx.ops.intgl
#elif defined(__SSE2__)
#   define vect_lrgst_singl_type    __m128
#   define vect_lrgst_doubl_type    __m128d

#   define vect_lrgst_intgl_type    __m128i
#   define vect_lrgst_intgl_oprts   &sse.ops.intgl
#else
#   define vect_lrgst_singl_type    simd64
#   define vect_lrgst_doubl_type    simd64d

#   define vect_lrgst_intgl_type    simd64i
#   define vect_lrgst_intgl_oprts   &simd.ops.intgl
#endif

static const struct {
    const _t(sse)  *const sse;
    const _t(avx)  *const avx;
    const _t(simd) *const simd;

    const struct {
        const struct {
#       if   defined(__AVX2__)
            const _t(avx.ops.intgl)
#       elif defined(__SSE2__)
            const _t(sse.ops.intgl)
#       else
            const _t(simd.ops.intgl)
#       endif
            *const ops;
        } intgl;
    } lrgst;
} vect = {
    .sse    = &sse,
    .avx    = &avx,
    .simd   = &simd,

    .lrgst.intgl.ops = vect_lrgst_intgl_oprts
};

#endif