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

#include "avl_tree.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

typedef struct StringView {
    const char *data;
    size_t length;
} StringView;

typedef struct String {
    char *data;
    size_t length;
} String;

typedef struct AvlTreeNode {
    struct AvlTreeNode *left;
    struct AvlTreeNode *right;
    struct AvlTreeNode *parent;
    size_t height;
    String key;
    void *value;
} AvlTreeNode;

struct AvlTreeIterator {
    AvlTreeNode *current;
};

struct AvlTree {
    AvlTreeNode *root;
    size_t size;
};

static AvlTreeErrorE StringView_init(StringView *self, const char *str, size_t len) {
    assert(self);
    assert(str);

    self->data = str;
    self->length = len;

    return AVL_SUCCESS;
}

static char* alloc_and_copy(StringView source) {
    char *const new_str = malloc(source.length + 1);

    if (!new_str) {
        return NULL;
    }

    memcpy(new_str, source.data, source.length);
    new_str[source.length + 1] = '\0';

    return new_str;
}

static AvlTreeErrorE String_destroy(String *self) {
    if (self->data) {
        free(self->data);
        self->data = NULL;
    }

    self->length = 0;

    return AVL_SUCCESS;
}

static AvlTreeErrorE StringView_copy(StringView self, String *dest) {
    assert(dest);

    char *const new_data = alloc_and_copy(self);

    if (!new_data) {
        return AVL_NO_MEMORY;
    }

    if (dest->data) {
        free(dest->data);
    }

    dest->data = new_data;
    dest->length = self.length;

    return AVL_SUCCESS;
}

static AvlTreeErrorE AvlTreeNode_init(AvlTreeNode *self, StringView key, void *value) {
    assert(self);

    const AvlTreeErrorE ret = StringView_copy(key, &self->key);

    if (ret != AVL_SUCCESS) {
        return ret;
    }

    self->left = NULL;
    self->right = NULL;
    self->parent = NULL;
    self->height = 0;
    self->value = value;

    return AVL_SUCCESS;
}

static AvlTreeErrorE AvlTreeNode_destroy(AvlTreeNode *self) {
    assert(self);

    if (self->left) {
        AvlTreeNode_destroy(self->left);
        self->left = NULL;
    }

    if (self->right) {
        AvlTreeNode_destroy(self->right);
        self->right = NULL;
    }

    self->parent = NULL;

    String_destroy(&self->key);
    self->value = NULL;

    return AVL_SUCCESS;
}

static AvlTreeErrorE AvlTreeNode_rotate_right(AvlTreeNode *self, AvlTreeNode **head) {
    assert(self);
    assert(self->left);

    AvlTreeNode *const left = self->left;

    self->left = left->right;

    if (self->left) {
        self->left->parent = self;
    }

    left->right = self;

    left->parent = self->parent;
    self->parent = left;

    if (left->parent) {
        if (left->parent->left == self) {
            left->parent->left = left;
        } else {
            left->parent->right = left;
        }
    }

    *head = left;

    return AVL_SUCCESS;
}

AvlTreeErrorE AvlTree_init(AvlTree *self) {
    assert(self);

    self->root = NULL;
    self->size = 0;

    return AVL_SUCCESS;
}

AvlTreeErrorE AvlTree_destroy(AvlTree *self) {
    assert(self);

    if (self->root) {
        AvlTreeNode_destroy(self->root);
        free(self->root);
        self->root = NULL;
    }

    self->size = 0;

    return AVL_SUCCESS;
}

static AvlTreeErrorE AvlTree_do_insert(AvlTree *self, AvlTreeNode *to_insert) {
    assert(self);
    assert(to_insert);

    if (!self->root) {
        self->root = to_insert;
    }

    ++self->size;

    return AVL_SUCCESS;
}

AvlTreeErrorE AvlTree_insert(AvlTree *self, const char *key, size_t key_length, void *value) {
    assert(self);
    assert(key);

    StringView key_view;
    StringView_init(&key_view, key, key_length);

    AvlTreeNode *const to_insert = malloc(sizeof(AvlTreeNode));

    if (!to_insert) {
        return AVL_NO_MEMORY;
    }

    const AvlTreeErrorE init_ret = AvlTreeNode_init(to_insert, key_view, value);

    if (init_ret != AVL_SUCCESS) {
        free(to_insert);

        return init_ret;
    }

    const AvlTreeErrorE insert_ret = AvlTree_do_insert(self, to_insert);

    if (insert_ret != AVL_SUCCESS) {
        free(to_insert);

        return insert_ret;
    }

    ++self->size;

    return AVL_SUCCESS;
}

AvlTreeErrorE AvlTree_erase(AvlTree *self, AvlTreeIterator iter) {
    return AVL_SUCCESS;
}

AvlTreeErrorE AvlTree_find(AvlTree *self, const char *key, size_t len, AvlTreeIterator *iter) {
    return AVL_SUCCESS;
}

