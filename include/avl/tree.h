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

#include <avl/error.h>
#include <avl/node.h>
#include <avl/types.h>

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Tree {
    TreeNode *root;
    size_t size;
    TreeComparatorT comparator;
} Tree;

TreeErrorE Tree_init(Tree *self, TreeComparatorT comparator);

TreeErrorE Tree_destroy(Tree *self);

TreeErrorE Tree_insert(Tree *self, const void *key, void *value);

TreeErrorE Tree_erase(Tree *self, const void *key);

TreeErrorE Tree_find(const Tree *self, const void *key, const void **value_ptr);

TreeErrorE Tree_find_mut(Tree *self, const void *key, void **value_ptr);

TreeErrorE Tree_clear(Tree *self);

TreeErrorE Tree_size(const Tree *self, size_t *size);

TreeErrorE Tree_traverse(const Tree *self, TreeTraversalCallbackT callback, void *context);

TreeErrorE Tree_traverse_mut(Tree *self, TreeMutTraversalCallbackT callback, void *context);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
