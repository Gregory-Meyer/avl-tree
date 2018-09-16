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

TreeErrorE Tree_init(Tree *self, TreeComparatorT comparator) {
    assert(self);

    self->root = NULL;
    self->size = 0;
    self->comparator = comparator;

    return TREE_SUCCESS;
}

TreeErrorE Tree_destroy(Tree *self) {
    assert(self);

    return Tree_clear(self);
}

TreeErrorE Tree_insert(Tree *self, const void *key, void *value) {
    assert(self);

    TreeNode *const to_insert = malloc(sizeof(TreeNode));

    if (!to_insert) {
        return TREE_NO_MEMORY;
    }

    const TreeErrorE init_ret = TreeNode_init(to_insert, key, value);

    if (init_ret != TREE_SUCCESS) {
        free(to_insert);

        return init_ret;
    }

    if (!self->root) {
        self->root = to_insert;
        ++self->size;

        return TREE_SUCCESS;
    }

    const TreeErrorE insert_ret = TreeNode_insert(self->root, to_insert, self->comparator);

    if (insert_ret != TREE_SUCCESS) {
        free(to_insert);

        return insert_ret;
    }

    for (; self->root->parent; self->root = self->root->parent) { }

    ++self->size;

    return TREE_SUCCESS;
}

TreeErrorE Tree_erase(Tree *self, const void *key) {
    assert(self);
    assert(key);

    if (!self->root) {
        return TREE_NO_SUCH_KEY;
    }

    TreeNode *found = NULL;
    const TreeErrorE lower_bound_ret =
        TreeNode_lower_bound(self->root, key, self->comparator, &found);

    if (lower_bound_ret != TREE_SUCCESS) {
        return lower_bound_ret;
    } else if (self->comparator(key, found->key) != 0) {
        return TREE_NO_SUCH_KEY;
    }

    const TreeErrorE erase_ret = TreeNode_erase(found);

    if (erase_ret != TREE_SUCCESS) {
        return erase_ret;
    }

    free(found);

    for (; self->root->parent; self->root = self->root->parent) { }
    --self->size;

    return TREE_SUCCESS;
}

TreeErrorE Tree_find(const Tree *self, const void *key, const void **value_ptr) {
    assert(self);
    assert(value_ptr);

    if (!self->root) {
        return TREE_NO_SUCH_KEY;
    }

    TreeNode *found = NULL;
    const TreeErrorE ret = TreeNode_lower_bound(self->root, key, self->comparator, &found);

    if (ret != TREE_SUCCESS) {
        return ret;
    } else if (self->comparator(found->key, key) != 0) {
        return TREE_NO_SUCH_KEY;
    }

    *value_ptr = found->value;

    return TREE_SUCCESS;
}

TreeErrorE Tree_find_mut(Tree *self, const void *key, void **value_ptr) {
    assert(self);
    assert(value_ptr);

    if (!self->root) {
        return TREE_NO_SUCH_KEY;
    }

    TreeNode *found = NULL;
    const TreeErrorE ret = TreeNode_lower_bound(self->root, key, self->comparator, &found);

    if (ret != TREE_SUCCESS) {
        return ret;
    } else if (self->comparator(found->key, key) != 0) {
        return TREE_NO_SUCH_KEY;
    }

    *value_ptr = found->value;

    return TREE_SUCCESS;
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

static TreeErrorE do_traverse(const TreeNode *self, TreeTraversalCallbackT callback,
                              void *context) {
    assert(self);

    if (self->left) {
        do_traverse(self->left, callback, context);
    }

    callback(context, self->key, self->value);

    if (self->right) {
        do_traverse(self->right, callback, context);
    }

    return TREE_SUCCESS;
}

static TreeErrorE do_traverse_mut(TreeNode *self, TreeMutTraversalCallbackT callback,
                                  void *context) {
    assert(self);

    if (self->left) {
        do_traverse_mut(self->left, callback, context);
    }

    callback(context, self->key, self->value);

    if (self->right) {
        do_traverse_mut(self->right, callback, context);
    }

    return TREE_SUCCESS;
}

TreeErrorE Tree_traverse(const Tree *self, TreeTraversalCallbackT callback, void *context) {
    assert(self);

    if (!self->root) {
        return TREE_SUCCESS;
    }

    return do_traverse(self->root, callback, context);
}

TreeErrorE Tree_traverse_mut(Tree *self, TreeMutTraversalCallbackT callback, void *context) {
    assert(self);

    if (!self->root) {
        return TREE_SUCCESS;
    }

    return do_traverse_mut(self->root, callback, context);
}
