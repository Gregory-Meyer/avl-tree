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

#include "internal/node.h"

#include "assert.h"

TreeErrorE TreeNode_init(TreeNode *self, const void *key, void *value) {
    assert(self);

    self->left = NULL;
    self->right = NULL;
    self->parent = NULL;
    self->height = 0;
    self->key = key;
    self->value = value;

    return TREE_SUCCESS;
}

TreeErrorE TreeNode_destroy(TreeNode *self) {
    assert(self);

    if (self->left) {
        TreeNode_destroy(self->left);
        self->left = NULL;
    }

    if (self->right) {
        TreeNode_destroy(self->right);
        self->right = NULL;
    }

    self->parent = NULL;
    self->key = NULL;
    self->value = NULL;

    return TREE_SUCCESS;
}

TreeErrorE TreeNode_rotate_right(TreeNode *self, TreeNode **head) {
    return TREE_NOT_IMPLEMENTED;
    // assert(self);
    // assert(self->left);

    // TreeNode *const left = self->left;

    // self->left = left->right;

    // if (self->left) {
    //     self->left->parent = self;
    // }

    // left->right = self;

    // left->parent = self->parent;
    // self->parent = left;

    // if (left->parent) {
    //     if (left->parent->left == self) {
    //         left->parent->left = left;
    //     } else {
    //         left->parent->right = left;
    //     }
    // }

    // *head = left;

    // return TREE_SUCCESS;
}

TreeErrorE TreeNode_rotate_left(TreeNode *self, TreeNode **head) {
    return TREE_NOT_IMPLEMENTED;
}

TreeErrorE TreeNode_find(TreeNode *self, const void *key,
                         TreeComparatorT comparator, TreeNode **found) {
    assert(self);
    assert(key);
    assert(found);

    const int compare = comparator(key, self->key);

    if (compare < 0) {
        if (!self->left) {
            return TREE_NO_SUCH_KEY;
        }

        return TreeNode_find(self->left, key, comparator, found);
    } else if (compare > 0) {
        if (!self->right) {
            return TREE_NO_SUCH_KEY;
        }

        return TreeNode_find(self->right, key, comparator, found);
    }

    *found = self;

    return TREE_SUCCESS;
}
