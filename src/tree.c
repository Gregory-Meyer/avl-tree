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

#include <avl/tree.h>

#include "internal/node.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

TreeErrorE Tree_init(Tree *self) {
    assert(self);

    self->root = NULL;
    self->size = 0;

    return TREE_SUCCESS;
}

TreeErrorE Tree_destroy(Tree *self) {
    assert(self);

    return Tree_clear(self);
}

static TreeErrorE do_insert(Tree *self, TreeNode *to_insert) {
    assert(self);
    assert(to_insert);

    if (!self->root) {
        self->root = to_insert;
    }

    ++self->size;

    return TREE_SUCCESS;
}

TreeErrorE Tree_insert(Tree *self, const char *key, size_t key_length, void *value) {
    assert(self);
    assert(key);

    StringView key_view;
    StringView_init(&key_view, key, key_length);

    TreeNode *const to_insert = malloc(sizeof(TreeNode));

    if (!to_insert) {
        return TREE_NO_MEMORY;
    }

    const TreeErrorE init_ret = TreeNode_init(to_insert, key_view, value);

    if (init_ret != TREE_SUCCESS) {
        free(to_insert);

        return init_ret;
    }

    const TreeErrorE insert_ret = do_insert(self, to_insert);

    if (insert_ret != TREE_SUCCESS) {
        free(to_insert);

        return insert_ret;
    }

    ++self->size;

    return TREE_SUCCESS;
}

TreeErrorE Tree_erase(Tree *self, const TreeIter *iter) {
    return TREE_NOT_IMPLEMENTED;
}

TreeErrorE Tree_find(const Tree *self, const char *key, size_t len, TreeIter *iter) {
    return TREE_NOT_IMPLEMENTED;
}

TreeErrorE Tree_find_mut(Tree *self, const char *key, size_t len, TreeIterMut *iter) {
    return TREE_NOT_IMPLEMENTED;
}

TreeErrorE Tree_clear(Tree *self) {
    assert(self);

    if (self->root) {
        TreeNode_destroy(self->root);
        free(self->root);
        self->root = NULL;
    }

    self->size = 0;

    return TREE_SUCCESS;
}

TreeErrorE Tree_size(const Tree *self, size_t *size) {
    assert(self);
    assert(size);

    *size = self->size;

    return TREE_SUCCESS;
}
