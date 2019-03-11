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
void AvlMap_init(AvlMap *self, AvlComparator compare, void *compare_arg,
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

static AvlNode* alloc_node(void *key, void *value);

static void rebalance(NodeStack *nodes);

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

        while (1) {
            const int compare = self->compare(key, current->key, self->compare_arg);

            if (compare == 0) { /* key == current */
                previous_value = current->value;
                current->value = value;

                break;
            } else if (compare < 0) { /* key < current */
                if (current->left) {
                    current = current->left;
                } else {
                    current->left = alloc_node(key, value);
                    ++self->len;

                    break;
                }
            } else {
                if (current->right) {
                    current = current->right;
                } else {
                    current->right = alloc_node(key, value);
                    ++self->len;

                    break;
                }
            }
        }

        if (previous_value) { /* no node inserted */
            NodeStack_destroy(&stack);

            return previous_value;
        }

        rebalance(&stack);

        NodeStack_destroy(&stack);

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

static int height(AvlNode *node);

static void rebalance(NodeStack *stack) {
    typedef enum ParentPath {
        NOPARENT,
        LEFT,
        RIGHT,
        LEFTLEFT,
        LEFTRIGHT,
        RIGHTLEFT,
        RIGHTRIGHT
    } ParentPath;

    AvlNode *current;

    assert(stack);

    for (current = NodeStack_pop(stack); current;) {
        AvlNode *const left = current->left;
        AvlNode *const right = current->right;

        const int left_height = height(current->left);
        const int right_height = height(current->right);

        const int balance_factor = right_height - left_height;

        current->height = MAX(left_height, right_height) + 1;

        if (balance_factor == -2 || balance_factor == 2) { /* uh oh */
            AvlNode *const parent = NodeStack_pop(stack);

            current = parent;
        } else {
            current = NodeStack_pop(stack);
        }
    }
}

static int height(AvlNode *node) {
    if (!node) {
        return 0;
    } else {
        return node->height;
    }
}

/**
 *  Destroys an AvlMap, removing all members.
 *
 *  Equivalent to AvlMap_clear. Runs in O(1) stack frames and O(n) time
 *  complexity.
 *
 *  @param self Must not be NULL. Must be initialized.
 */
void AvlMap_destroy(AvlMap *self) {
    assert(self);

    AvlMap_clear(self);
}

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

    /* Morris in-order tree traversal */
    current = self->root;
    while (current) {
        if (!current->left) {
            AvlNode *const next = current->right;

            self->deleter(current->key, current->value, self->deleter_arg);
            free(current);

            current = next;
        } else {
            /* rightmost node of tree with root current->left */
            AvlNode *predecessor = current->left;

            while (predecessor->right && predecessor->right != current) {
                predecessor = predecessor->right;
            }

            if (!predecessor->right) {
                predecessor->right = current;
                current = current->left;
            } else { /* predecessor->right == current */
                AvlNode *const next = current->right;

                self->deleter(current->key, current->value, self->deleter_arg);
                free(current);

                predecessor->right = NULL;
                current = next;
            }
        }
    }

    self->len = 0;
}
