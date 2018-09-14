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

static TreeErrorE do_insert(TreeNode *node, TreeNode *to_insert, TreeComparatorT comparator) {
    assert(node);
    assert(to_insert);

    const int compare = comparator(to_insert->key, node->key);

    if (compare < 0) {
        if (!node->left) {
            node->left = to_insert;
            to_insert->parent = node;

            return TREE_SUCCESS;
        }

        return do_insert(node->left, to_insert, comparator);
    } else if (compare > 0) {
        if (!node->right) {
            node->right = to_insert;
            to_insert->parent = node;

            return TREE_SUCCESS;
        }

        return do_insert(node->right, to_insert, comparator);
    }

    return TREE_DUPLICATE_KEY;
}

TreeErrorE Tree_insert(Tree *self, const void *key, void *value) {
    assert(self);
    assert(key);

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

    const TreeErrorE insert_ret = do_insert(self->root, to_insert, self->comparator);

    if (insert_ret != TREE_SUCCESS) {
        free(to_insert);

        return insert_ret;
    }

    ++self->size;

    return TREE_SUCCESS;
}

static TreeNode* inorder_successor(TreeNode *node) {
    assert(node);

    if (node->right) {
        for (node = node->right; node->left; node = node->left) { }

        return node;
    }

    for (; node->parent && node->parent->right == node; node = node->parent) { }

    return node->parent;
}

TreeErrorE Tree_erase(Tree *self, const void *key) {
    return TREE_NOT_IMPLEMENTED;

    // assert(self);
    // assert(key);

    // if (!self->root) {
    //     return TREE_NO_SUCH_KEY;
    // }

    // TreeNode *found = NULL;
    // const TreeErrorE ret = TreeNode_find(self->root, key, self->comparator, &found);

    // if (ret != TREE_SUCCESS) {
    //     return ret;
    // }

    // TreeNode *successor = inorder_successor(found);

    // if (successor->parent->left == successor) {
    //     successor->parent->left = NULL;
    // } else {
    //     successor->parent->right = NULL;
    // }

    // return TREE_SUCCESS;
}

TreeErrorE Tree_find(const Tree *self, const void *key, const void **value) {
    assert(self);
    assert(key);
    assert(value);

    if (!self->root) {
        return TREE_NO_SUCH_KEY;
    }

    TreeNode *found = NULL;
    const TreeErrorE ret = TreeNode_find(self->root, key, self->comparator, &found);

    if (ret != TREE_SUCCESS) {
        return ret;
    }

    *value = found->value;

    return TREE_SUCCESS;
}

TreeErrorE Tree_find_mut(Tree *self, const void *key, void **value) {
    assert(self);
    assert(key);
    assert(value);

    if (!self->root) {
        return TREE_NO_SUCH_KEY;
    }

    TreeNode *found = NULL;
    const TreeErrorE ret = TreeNode_find(self->root, key, self->comparator, &found);

    if (ret != TREE_SUCCESS) {
        return ret;
    }

    *value = found->value;

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
