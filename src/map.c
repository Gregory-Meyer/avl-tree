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

#include <bloodhound.h>

#include "mem.h"
#include "node_stack.h"

#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stdlib.h>

#define MAX(X, Y) (((X) < (Y)) ? (Y) : (X))

/** AVL Tree node. */
struct AvlNode {
    AvlNode *left;
    AvlNode *right;
    void *key;
    void *value;
    signed char balance_factor; /* on a 64-bit architecture this is <= 90 */
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

/* unsigned long is >= 32 bits, 3 words gives at least 96 bits
 * maximum AVL tree depth is 1.44 log_2(n), so if there are 2^63 - 1
 * nodes, the tree will be at most 90.72 nodes deep
 */
#define NUM_FLAG_WORDS 3

static void rebalance(const unsigned long (*is_left_flags)[NUM_FLAG_WORDS],
                      AvlNode **root_ptr, AvlNode *inserted);

static int getbit(const unsigned long *arr, size_t bit);

static void setbit(unsigned long *arr, size_t bit);

static void clearbit(unsigned long *arr, size_t bit);

#ifdef NDEBUG
#define assert_correct_balance_factors(N) ((void) 0)
#else
#define assert_correct_balance_factors(N) do_assert_balance_factors((N))
static int do_assert_balance_factors(const AvlNode *node);
#endif

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
        unsigned long is_left_flags[NUM_FLAG_WORDS];
        AvlNode **current_ptr = &self->root;
        AvlNode **rotate_root_ptr = &self->root;
        void *previous_value = NULL;
        size_t current_depth_from_root = 0;

        while (1) {
            AvlNode *const current = *current_ptr;
            const int compare = self->compare(key, current->key, self->compare_arg);

            if (compare == 0) { /* key == current */
                previous_value = current->value;
                current->value = value;

                return previous_value;
            } else {
                if (current->balance_factor != 0) {
                    rotate_root_ptr = current_ptr;
                    current_depth_from_root = 0;
                }

                if (compare < 0) { /* key < current */
                    setbit(is_left_flags, current_depth_from_root);
                    current_ptr = &current->left;
                } else { /* key > current, compare > 0 */
                    clearbit(is_left_flags, current_depth_from_root);
                    current_ptr = &current->right;
                }

                ++current_depth_from_root;

                if (!*current_ptr) {
                    *current_ptr = alloc_node(key, value);
                    assert((*current_ptr)->balance_factor == 0);
                    ++self->len;

                    break;
                }
            }
        }

        rebalance(&is_left_flags, rotate_root_ptr, *current_ptr);
        assert_correct_balance_factors(self->root);

        return NULL;
    }
}

static AvlNode* alloc_node(void *key, void *value) {
    AvlNode *const node = (AvlNode*) checked_calloc(1, sizeof(AvlNode));

    node->balance_factor = 0;
    node->key = key;
    node->value = value;

    return node;
}

static AvlNode* rotate(AvlNode *top, AvlNode *middle_or_bottom);

static void rebalance(const unsigned long (*is_left_flags)[NUM_FLAG_WORDS],
                      AvlNode **root_ptr, AvlNode *inserted) {
    AvlNode *current;
    size_t depth_from_root;

    assert(is_left_flags);
    assert(root_ptr);
    assert(*root_ptr);

    for (depth_from_root = 0, current = *root_ptr; current != inserted; ++depth_from_root) {
        if (getbit(*is_left_flags, depth_from_root)) {
            --current->balance_factor;
            current = current->left;
        } else {
            ++current->balance_factor;
            current = current->right;
        }
    }

    *root_ptr = rotate(*root_ptr, getbit(*is_left_flags, 0) ? (*root_ptr)->left
                                                            : (*root_ptr)->right);
}

static int getbit(const unsigned long *arr, size_t bit) {
    static const size_t NBITS = CHAR_BIT * sizeof(unsigned long);

    const size_t idx = bit / NBITS;
    const size_t subidx = bit % NBITS;

    return (arr[idx] & (1ul << subidx)) != 0;
}

static void setbit(unsigned long *arr, size_t bit) {
    static const size_t NBITS = CHAR_BIT * sizeof(unsigned long);

    const size_t idx = bit / NBITS;
    const size_t subidx = bit % NBITS;

    arr[idx] |= (1ul << subidx);
}

static void clearbit(unsigned long *arr, size_t bit) {
    static const size_t NBITS = CHAR_BIT * sizeof(unsigned long);

    const size_t idx = bit / NBITS;
    const size_t subidx = bit % NBITS;

    arr[idx] &= ~(1ul << subidx);
}

static AvlNode* rotate_leftright(AvlNode *top, AvlNode *middle, AvlNode *bottom);

static AvlNode* rotate_rightleft(AvlNode *top, AvlNode *middle, AvlNode *bottom);

static AvlNode* rotate_left(AvlNode *top, AvlNode *bottom);

static AvlNode* rotate_right(AvlNode *top, AvlNode *bottom);

static AvlNode* rotate(AvlNode *top, AvlNode *middle_or_bottom) {
    AvlNode *root;

    assert(top);
    assert(middle_or_bottom);
    assert(top->left == middle_or_bottom || top->right == middle_or_bottom);

    if (top->balance_factor == -2) {
        assert(top->left == middle_or_bottom);

        if (middle_or_bottom->balance_factor == -1) {
            root = rotate_right(top, middle_or_bottom);

            top->balance_factor = 0;
            middle_or_bottom->balance_factor = 0;
        } else { /* middle_or_bottom->balance_factor == 1 */
            AvlNode *child;

            assert(middle_or_bottom->balance_factor == 1);
            assert(middle_or_bottom->right);

            child = middle_or_bottom->right;
            root = rotate_leftright(top, middle_or_bottom, child);

            if (child->balance_factor == -1) {
                top->balance_factor = 1;
                middle_or_bottom->balance_factor = 0;
            } else if (child->balance_factor == 0) {
                top->balance_factor = 0;
                middle_or_bottom->balance_factor = 0;
            } else {
                assert(child->balance_factor == 1);

                top->balance_factor = 0;
                middle_or_bottom->balance_factor = -1;
            }

            child->balance_factor = 0;
        }
    } else if (top->balance_factor == 2) {
        assert(top->right == middle_or_bottom);

        if (middle_or_bottom->balance_factor == 1) {
            root = rotate_left(top, middle_or_bottom);

            top->balance_factor = 0;
            middle_or_bottom->balance_factor = 0;
        } else { /* middle_or_bottom->balance_factor == -1 */
            AvlNode *child;

            assert(middle_or_bottom->balance_factor == -1);
            assert(middle_or_bottom->left);

            child = middle_or_bottom->left;
            root = rotate_rightleft(top, middle_or_bottom, child);

            if (child->balance_factor == 1) {
                top->balance_factor = -1;
                middle_or_bottom->balance_factor = 0;
            } else if (child->balance_factor == 0) {
                top->balance_factor = 0;
                middle_or_bottom->balance_factor = 0;
            } else {
                assert(child->balance_factor == -1);

                top->balance_factor = 0;
                middle_or_bottom->balance_factor = 1;
            }

            child->balance_factor = 0;
        }
    } else {
        root = top;
    }

    return root;
}

static AvlNode* rotate_leftright(AvlNode *top, AvlNode *middle, AvlNode *bottom) {
    AvlNode *root;

    assert(top);
    assert(middle);
    assert(bottom);
    assert(top->left == middle);
    assert(middle->right == bottom);
    assert(top->balance_factor == -2);
    assert(middle->balance_factor == 1);

    top->left = rotate_left(middle, bottom);
    root = rotate_right(top, bottom);

    return root;
}

static AvlNode* rotate_rightleft(AvlNode *top, AvlNode *middle, AvlNode *bottom) {
    AvlNode *root;

    assert(top);
    assert(middle);
    assert(bottom);
    assert(top->right == middle);
    assert(middle->left == bottom);
    assert(top->balance_factor == 2);
    assert(middle->balance_factor == -1);

    top->right = rotate_right(middle, bottom);
    root = rotate_left(top, bottom);

    return root;
}

static AvlNode* rotate_left(AvlNode *top, AvlNode *bottom) {
    assert(top);
    assert(bottom);
    assert(top->right == bottom);

    top->right = bottom->left;
    bottom->left = top;

    return bottom;
}

static AvlNode* rotate_right(AvlNode *top, AvlNode *bottom) {
    assert(top);
    assert(bottom);
    assert(top->left == bottom);

    top->left = bottom->right;
    bottom->right = top;

    return bottom;
}

#ifndef NDEBUG
static int do_assert_balance_factors(const AvlNode *node) {
    if (!node) {
        return 0;
    } else {
        const int left_height = assert_correct_balance_factors(node->left);
        const int right_height = assert_correct_balance_factors(node->right);

        const signed char balance_factor = (signed char) (right_height - left_height);

        assert(node->balance_factor == balance_factor);

        return MAX(left_height, right_height) + 1;
    }
}
#endif

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
