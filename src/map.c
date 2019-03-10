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

#include <assert.h>
#include <stdlib.h>

/** AVL Tree node. */
struct AvlNode {
    AvlNode *left;
    AvlNode *right;
    void *key;
    void *value;
    /* If not one of (-1, 0, 1), needs to be rebalanced. */
    signed char balance_factor;
};

/**
 *  Initializes an empty AvlMap.
 *
 *  @param self Must not be NULL. Must have not been initialized yet.
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

/**
 *  Destroys an AvlMap, removing all members.
 *
 *  Equivalent to AvlMap_clear. Runs in O(1) stack frames and O(n) time
 *  complexity.
 *
 *  @param self Must not be NULL. Must have been initialized by
 *              AvlMap_init and must not have been destroyed yet by
 *              AvlMap_destroy.
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
 *  @param self Must not be NULL. Must have been initialized by
 *              AvlMap_init.
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
}
