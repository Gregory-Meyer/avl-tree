// BSD 3-Clause License
//
// Copyright (c) 2018, Gregory Meyer
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS AS IS
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#ifndef AVL_TREE_H
#define AVL_TREE_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum AvlTreeErrorE {
    AVL_SUCCESS,
    AVL_NULL_KEY,
    AVL_NOT_FOUND,
    AVL_NO_MEMORY
} AvlTreeErrorE;

typedef struct AvlTree AvlTree;

typedef struct AvlTreeIterator AvlTreeIterator;

AvlTreeErrorE AvlTree_init(AvlTree *self);

AvlTreeErrorE AvlTree_destroy(AvlTree *self);

AvlTreeErrorE AvlTree_insert(AvlTree *self, const char *key, size_t len, void *value);

AvlTreeErrorE AvlTree_erase(AvlTree *self, AvlTreeIterator iter);

AvlTreeErrorE AvlTree_find(AvlTree *self, const char *key, size_t len, AvlTreeIterator *iter);

AvlTreeErrorE AvlTreeIterator_next(AvlTreeIterator *self);

AvlTreeErrorE AvlTreeIterator_prev(AvlTreeIterator *self);

AvlTreeErrorE AvlTreeIterator_deref(const AvlTreeIterator *self, void **value);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
