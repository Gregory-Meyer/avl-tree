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

#include "bit_stack.h"
#include "mem.h"
#include "node.h"
#include "node_stack.h"

#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stdlib.h>

#define MAX(X, Y) (((X) < (Y)) ? (Y) : (X))

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

static void rebalance(const BitStack *is_left_flags, AvlNode **root_ptr, AvlNode *inserted);

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
        BitStack is_left_flags;
        AvlNode **current_ptr = &self->root;
        AvlNode **rotate_root_ptr = &self->root;
        void *previous_value = NULL;

        BitStack_new(&is_left_flags);

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
                    BitStack_clear(&is_left_flags);
                }

                if (compare < 0) { /* key < current */
                    BitStack_push_set(&is_left_flags);
                    current_ptr = &current->left;
                } else { /* key > current, compare > 0 */
                    BitStack_push_clear(&is_left_flags);
                    current_ptr = &current->right;
                }

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

        BitStack_drop(&is_left_flags);

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

static void rebalance(const BitStack *is_left_flags, AvlNode **root_ptr, AvlNode *inserted) {
    AvlNode *current;
    size_t depth_from_root;

    assert(is_left_flags);
    assert(root_ptr);
    assert(*root_ptr);

    for (depth_from_root = BitStack_len(is_left_flags) - 1, current = *root_ptr;
         current != inserted; --depth_from_root) {
        const int is_left = BitStack_get(is_left_flags, depth_from_root);
        assert(is_left != -1);

        if (is_left) {
            --current->balance_factor;
            current = current->left;
        } else {
            ++current->balance_factor;
            current = current->right;
        }
    }

    *root_ptr = rotate(*root_ptr);
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

static void remove_node(AvlMap *self, AvlNode **node_ptr, NodeStack *nodes,
                        BitStack *is_left_flags);

/**
 *  Removes the value associated with a key as well as the key that
 *  compared equal.
 *
 *  @param self Must not be NULL.
 *  @returns Nonzero if a (key, value) pair was removed from this map,
 *           zero otherwise.
 */
int AvlMap_remove(AvlMap *self, const void *key) {
    NodeStack nodes;
    BitStack is_left_flags;
    size_t current_depth = 0;
    AvlNode **current_ptr;

    assert(self);

    NodeStack_new(&nodes);
    BitStack_new(&is_left_flags);
    for (current_ptr = &self->root; *current_ptr; ++current_depth) {
        AvlNode *const current = *current_ptr;
        int ordering;

        NodeStack_push(&nodes, current);

        ordering = self->compare(key, current->key, self->compare_arg);

        if (ordering == 0) {
            break; /* we got em */
        } else if (ordering < 0 && current->left) {
            current_ptr = &current->left;
            BitStack_push_set(&is_left_flags);
        } else if (ordering > 0 && current->right) {
            current_ptr = &current->right;
            BitStack_push_clear(&is_left_flags);
        } else {
            BitStack_drop(&is_left_flags);
            NodeStack_drop(&nodes);

            return 0; /* no such key... */
        }
    }

    remove_node(self, current_ptr, &nodes, &is_left_flags);

    BitStack_drop(&is_left_flags);
    NodeStack_drop(&nodes);

    return 1;
}

static AvlNode* swap_for_delete(NodeStack *nodes, BitStack *is_left_flags, AvlNode *node);

static void update_balance_factors_and_rebalance(AvlMap *self, NodeStack *nodes,
                                                 BitStack *is_left_flags);

static void remove_node(AvlMap *self, AvlNode **node_ptr, NodeStack *nodes,
                        BitStack *is_left_flags) {
    AvlNode *node;

    assert(self);
    assert(node_ptr);
    assert(*node_ptr);
    assert(nodes);

    node = *node_ptr;

    if (node->left && node->right) {
        *node_ptr = swap_for_delete(nodes, is_left_flags, *node_ptr);
    } else if (node->left) {
        *node_ptr = node->left;
        node->left = NULL;
    } else if (node->right) {
        *node_ptr = node->right;
        node->right = NULL;
    } else {
        *node_ptr = NULL;
    }

    self->deleter(node->key, node->value, self->deleter_arg);
    free(node);

    NodeStack_pop(nodes);

    update_balance_factors_and_rebalance(self, nodes, is_left_flags);
    assert_correct_balance_factors(self->root);
}

/* find inorder sucessor */
static AvlNode* swap_for_delete(NodeStack *nodes, BitStack *is_left_flags, AvlNode *node) {
    AvlNode **successor_ptr;
    AvlNode *successor;
    size_t swap_idx;

    assert(nodes);
    assert(is_left_flags);
    assert(node);
    assert(node->left && node->right);

    successor_ptr = &node->right;
    BitStack_push_clear(is_left_flags);
    swap_idx = NodeStack_push(nodes, NULL) - 1;

    while ((*successor_ptr)->left) {
        successor_ptr = &(*successor_ptr)->left;
        BitStack_push_set(is_left_flags);
        NodeStack_push(nodes, *successor_ptr);
    }

    successor = *successor_ptr;
    *successor_ptr = successor->right;
    successor->left = node->left;
    successor->right = node->right;
    successor->balance_factor = node->balance_factor;

    node->left = NULL;
    node->right = NULL;
    node->balance_factor = 0;

    *NodeStack_get_mut(nodes, (ptrdiff_t) swap_idx) = successor;

    BitStack_pop(is_left_flags);
    NodeStack_pop(nodes);

    return successor;
}

static void update_balance_factors_and_rebalance(AvlMap *self, NodeStack *nodes,
                                                 BitStack *is_left_flags) {
    assert(nodes);
    assert(is_left_flags);

    while (1) {
        AvlNode *const node = NodeStack_pop(nodes);
        const int is_left = BitStack_pop(is_left_flags);
        const int parent_dir = BitStack_get(is_left_flags, 0);
        AvlNode **parent_ptr;

        if (!node || is_left == -1) {
            break;
        }

        if (parent_dir == -1) {
            parent_ptr = &self->root;
        } else {
            AvlNode *const parent = NodeStack_get(nodes, 0);
            assert(parent);

            if (parent_dir) {
                parent_ptr = &parent->left;
            } else {
                parent_ptr = &parent->right;
            }
        }

        assert(node == *parent_ptr);

        if (is_left) {
            ++node->balance_factor;

            if (node->balance_factor == 1) {
                return;
            } else if (node->balance_factor == 2) {
                *parent_ptr = rotate(node);
            }
        } else if (!is_left) {
            --node->balance_factor;

            if (node->balance_factor == -1) {
                return;
            } else if (node->balance_factor == -2) {
                *parent_ptr = rotate(node);
            }
        }
    }
}

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
            current = rotate_right_unchecked(current, current->left);
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
