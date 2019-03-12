/*  MIT License
 *
 *  Copyright (c) 2019 Gregory Meyer
 *
 *  Permission is hereby granted, free of charge, to any person
 *  obtaining a copy of this software and associated documentation
 *  files (the "Software"), to deal in the Software without
 *  restriction, including without limitation the rights to use, copy,
 *  modify, merge, publish, distribute, sublicense, and/or sell copies
 *  of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice (including
 *  the next paragraph) shall be included in all copies or substantial
 *  portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 *  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 *  ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 *  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 */

#include <avlbst.h>

#include "mem.h"
#include "node_stack.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX(X, Y) (((X) < (Y)) ? (Y) : (X))

/** AVL Tree node. */
struct AvlNode {
    AvlNode *left;
    AvlNode *right;
    void *key;
    void *value;
    int height; /* 0 <= height <= 64 due to AVL properties */
};

/**
 *  Initializes an empty AvlMap.
 *
 *  @param self Must not be NULL. Must not be initialized.
 *  @param compare Must not be NULL. Will be used to compare keys as if
 *                 by compare(lhs, rhs, compare_arg). Return values
 *                 should have the same meaning as strcmp and should
 *                 form a total order over the set of keys.
 *  @param compare_arg Passed to compare when invoked.
 *  @param deleter Must not be NULL. Will be used to free key-value
 *                 pairs when they are no longer usable by the tree as
 *                 if by deleter(key, value, deleter_arg).
 *  @param deleter_arg Passed to deleter when invoked.
 */
void AvlMap_new(AvlMap *self, AvlComparator compare, void *compare_arg,
                AvlDeleter deleter, void *deleter_arg) {
    assert(self);
    assert(compare);
    assert(deleter);

    self->root = NULL;
    self->len = 0;
    self->compare = compare;
    self->compare_arg = compare_arg;
    self->deleter = deleter;
    self->deleter_arg = deleter_arg;
}

/**
 *  Drops an AvlMap, removing all members.
 *
 *  Equivalent to AvlMap_clear. Runs in O(1) stack frames and O(n) time
 *  complexity.
 *
 *  @param self Must not be NULL. Must be initialized.
 */
void AvlMap_drop(AvlMap *self) {
    assert(self);

    AvlMap_clear(self);
}

static AvlNode* find(const AvlMap *self, const void *key);

static void* value(AvlNode *node);

/**
 *  @param self Must not be NULL. Must be initialized.
 *  @returns A pointer to the value associated with key. If no such
 *           value is in self, NULL.
 */
const void* AvlMap_get(const AvlMap *self, const void *key) {
    assert(self);

    return value(find(self, key));
}

/**
 *  @param self Must not be NULL. Must be initialized.
 *  @returns A mutable pointer to the value associated with key. If no
 *           such value is in self, NULL.
 */
void* AvlMap_get_mut(AvlMap *self, const void *key) {
    assert(self);

    return value(find(self, key));
}

static AvlNode* find(const AvlMap *self, const void *key) {
    AvlNode *current;

    assert(self);

    current = self->root;

    while (current) {
        const int compare = self->compare(key, current->key, self->compare_arg);

        if (compare == 0) {
            return current;
        } else if (compare < 0) {
            current = current->left;
        } else { /* compare > 0 */
            current = current->right;
        }
    }

    return NULL;
}

static void* value(AvlNode *node) {
    if (!node) {
        return NULL;
    }

    return node->value;
}

static AvlNode* alloc_node(void *key, void *value);

static size_t max_height(size_t num_nodes);

static void rebalance(AvlMap *const self, const NodeStack *stack);

/**
 *  Inserts a (key, value) pair into an AvlMap, taking ownership of
 *  them.
 *
 *  @param self Must not be NULL. Must be initialized.
 *  @param key If a key in this AvlMap compares equal, ownership will
 *             remain with the caller and will not be transferred to
 *             the AvlMap.
 *  @returns The previous value associated with key, if it exists.
 *           Ownership is transferred back to the caller in this case.
 */
void* AvlMap_insert(AvlMap *self, void *key, void *value) {
    assert(self);

    if (!self->root) {
        self->root = alloc_node(key, value);
        ++self->len;

        return NULL;
    } else {
        NodeStack stack;
        AvlNode *current = self->root;
        void *previous_value = NULL;

        NodeStack_with_capacity(&stack, max_height(self->len) + 1);

        while (1) {
            const int compare = self->compare(key, current->key, self->compare_arg);

            if (compare == 0) { /* key == current */
                NodeStack_drop(&stack);

                previous_value = current->value;
                current->value = value;

                return previous_value;
            } else {
                AvlNode **const to_compare = (compare < 0) ? &current->left : &current->right;
                NodeStack_push(&stack, current);

                if (*to_compare) {
                    current = *to_compare;
                } else {
                    AvlNode *const inserted = alloc_node(key, value);
                    NodeStack_push(&stack, inserted);
                    *to_compare = inserted;
                    ++self->len;

                    break;
                }
            }
        }

        rebalance(self, &stack);
        NodeStack_drop(&stack);

        return NULL;
    }
}

static AvlNode* alloc_node(void *key, void *value) {
    AvlNode *const node = (AvlNode*) checked_calloc(1, sizeof(AvlNode));

    node->height = 1;
    node->key = key;
    node->value = value;

    return node;
}

static size_t max_height(size_t num_nodes) {
    return (size_t) ceil(1.44 * log((double) num_nodes) / log(2.0));
}

static int update_height(AvlNode *node);

static AvlNode* rotate(AvlNode *top, AvlNode *middle, AvlNode *bottom);

static void rebalance(AvlMap *const self, const NodeStack *stack) {
    AvlNode *current;
    size_t index;

    assert(self);
    assert(stack);

    for (index = 0, current = NodeStack_get(stack, index); current;
         current = NodeStack_get(stack, ++index)) {
        const int balance_factor = update_height(current);

        if (balance_factor == -2 || balance_factor == 2) {
            AvlNode *const parent = NodeStack_get(stack, index + 1);

            AvlNode *const top = current;
            AvlNode *const middle = NodeStack_get(stack, index - 1);
            AvlNode *const bottom = NodeStack_get(stack, index - 2);

            AvlNode *const new_root = rotate(top, middle, bottom);

            AvlNode **root_to_set;
            if (parent) {
                root_to_set = (parent->left == top) ? &parent->left : &parent->right;
            } else {
                root_to_set = &self->root;
            }

            *root_to_set = new_root;
        }
    }
}

static AvlNode* rotate_leftright(AvlNode *top, AvlNode *middle, AvlNode *bottom);

static AvlNode* rotate_rightleft(AvlNode *top, AvlNode *middle, AvlNode *bottom);

static AvlNode* rotate_left(AvlNode *top, AvlNode *bottom);

static AvlNode* rotate_right(AvlNode *top, AvlNode *bottom);

static AvlNode* rotate(AvlNode *top, AvlNode *middle, AvlNode *bottom) {
    assert(top);
    assert(middle);
    assert(bottom);
    assert(top->left == middle || top->right == middle);
    assert(middle->left == bottom || middle->right == bottom);

    if (top->left == middle) {
        if (middle->left == bottom) { /* left left */
            return rotate_right(top, middle);
        } else { /* left right, middle->right == bottom */
            return rotate_leftright(top, middle, bottom);
        }
    } else { /* top->right == middle */
        if (middle->left == bottom) { /* right left */
            return rotate_rightleft(top, middle, bottom);
        } else { /* right right, middle->right == bottom */
            return rotate_left(top, middle);
        }
    }
}

static AvlNode* rotate_leftright(AvlNode *top, AvlNode *middle, AvlNode *bottom) {
    assert(top);
    assert(middle);
    assert(bottom);
    assert(top->left == middle);
    assert(middle->right == bottom);

    top->left = rotate_left(middle, bottom);

    return rotate_right(top, bottom);
}

static AvlNode* rotate_rightleft(AvlNode *top, AvlNode *middle, AvlNode *bottom) {
    assert(top);
    assert(middle);
    assert(bottom);
    assert(top->right == middle);
    assert(middle->left == bottom);

    top->right = rotate_right(middle, bottom);

    return rotate_left(top, bottom);
}

static AvlNode* rotate_left(AvlNode *top, AvlNode *bottom) {
    assert(top);
    assert(bottom);
    assert(top->right == bottom);

    top->right = bottom->left;
    bottom->left = top;
    update_height(top); /* top is now bottom */
    update_height(bottom); /* bottom is now top */

    return bottom;
}

static AvlNode* rotate_right(AvlNode *top, AvlNode *bottom) {
    assert(top);
    assert(bottom);
    assert(top->left == bottom);

    top->left = bottom->right;
    bottom->right = top;
    update_height(top); /* top is now bottom */
    update_height(bottom); /* bottom is now top */

    return bottom;
}

static int height(AvlNode *node);

static int update_height(AvlNode *node) {
    const int left_height = height(node->left);
    const int right_height = height(node->right);

    const int balance_factor = right_height - left_height;

    node->height = MAX(left_height, right_height) + 1;

    return balance_factor;
}

static int height(AvlNode *node) {
    if (!node) {
        return 0;
    } else {
        return node->height;
    }
}

/**
 *  Removes the value associated with a key as well as the key that
 *  compared equal.
 *
 *  @param self Must not be NULL.
 *  @returns Nonzero if a (key, value) pair was removed from this map,
 *           zero otherwise.
 */
int AvlMap_remove(AvlMap *self, const void *key);

static void drop(AvlMap *self, AvlNode *node);

/**
 *  Clears the map, removing all members.
 *
 *  Runs in O(1) stack frames and O(n) time complexity.
 *
 *  @param self Must not be NULL. Must be initialized.
 */
void AvlMap_clear(AvlMap *self) {
    AvlNode *current;

    assert(self);

    if (!self->root) {
        return;
    }

    current = self->root;
    while (current) {
        AvlNode *next;

        while (current->left) {
            current = rotate_right(current, current->left);
        }

        next = current->right;
        drop(self, current);
        current = next;
    }

    self->len = 0;
    self->root = NULL;
}

static void drop(AvlMap *self, AvlNode *node) {
    self->deleter(node->key, node->value, self->deleter_arg);
    free(node);
}
