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
#include <stdio.h>
#include <stdlib.h>

#define MAX(X, Y) (((X) < (Y)) ? (Y) : (X))

/**
 *  Initializes an empty AvlMap.
 *
 *  @param self Must not be NULL. Must not be initialized.
 *  @param compare Must not be NULL. Will be used to compare keys as if
 *                 by compare(lhs, rhs, compare_arg). Return values
 *                 should have the same meaning as strcmp and should
 *                 form a total ordering over the set of keys.
 *  @param deleter Must not be NULL. Will be used to free nodes when
 *                 they are no longer usable by the tree as if by
 *                 deleter(node, deleter_arg).
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

static AvlNode* find(const AvlMap *self, const void *key, AvlHetComparator comparator,
                     void *arg);

/**
 *  @param self Must not be NULL. Must be initialized.
 *  @param comparator Must not be NULL. Must form the same total
 *                    ordering over the contained elements as the one
 *                    formed by the comparator passed to AvlMap_new.
 *                    Will be invoked by comparator(key, node, arg).
 *  @returns A pointer to the node that compares equal to key, if
 *           there is one.
 */
const AvlNode* AvlMap_get(const AvlMap *self, const void *key, AvlHetComparator comparator,
                          void *arg) {
    assert(self);
    assert(comparator);

    return find(self, key, comparator, arg);
}

/**
 *  @param self Must not be NULL. Must be initialized.
 *  @param comparator Must not be NULL. Must form the same total
 *                    ordering over the contained elements as the one
 *                    formed by the comparator passed to AvlMap_new.
 *                    Will be invoked by comparator(key, node, arg).
 *  @returns A mutable pointer to the node that compares equal to key,
 *           if there is one.
 */
AvlNode* AvlMap_get_mut(AvlMap *self, const void *key, AvlHetComparator comparator, void *arg) {
    assert(self);
    assert(comparator);

    return find(self, key, comparator, arg);
}

static AvlNode* find(const AvlMap *self, const void *key, AvlHetComparator comparator,
                     void *arg) {
    AvlNode *current;

    assert(self);

    current = self->root;

    while (current) {
        const int compare = comparator(key, current, arg);

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

static void rebalance(const BitStack *is_left_flags, AvlNode **root_ptr, AvlNode *inserted);

#ifdef NDEBUG
#define assert_correct_balance_factors(N) ((void) 0)
#else
#define assert_correct_balance_factors(N) do_assert_balance_factors((N))
static int do_assert_balance_factors(const AvlNode *node);
#endif

#define IS_LEFT_FLAGS_BUF_SZ 3

/**
 *  Inserts an element into an AvlMap.
 *
 *  @param self Must not be NULL. Must be initialized.
 *  @param node Must not be NULL.
 *  @returns The previous element that compares equal to node, if there
 *           was one.
 */
AvlNode* AvlMap_insert(AvlMap *self, AvlNode *node) {
    assert(self);
    assert(node);

    if (!self->root) {
        self->root = node;
        ++self->len;

        return NULL;
    } else {
        unsigned long is_left_flags_buf[IS_LEFT_FLAGS_BUF_SZ];
        BitStack is_left_flags;
        AvlNode **current_ptr = &self->root;
        AvlNode **rotate_root_ptr = &self->root;

        BitStack_from_adopted_slice(&is_left_flags, is_left_flags_buf, IS_LEFT_FLAGS_BUF_SZ);

        while (1) {
            AvlNode *const current = *current_ptr;
            const int compare = self->compare(node, current, self->compare_arg);

            if (compare == 0) { /* node == current */
                *current_ptr = node;
                node->left = current->left;
                node->right = current->right;
                node->balance_factor = current->balance_factor;

                current->left = NULL;
                current->right = NULL;
                current->balance_factor = 0;

                BitStack_drop(&is_left_flags);

                return current;
            } else {
                if (current->balance_factor != 0) {
                    rotate_root_ptr = current_ptr;
                    BitStack_clear(&is_left_flags);
                }

                if (compare < 0) { /* node < current */
                    BitStack_push_set(&is_left_flags);
                    current_ptr = &current->left;
                } else { /* node > current, compare > 0 */
                    BitStack_push_clear(&is_left_flags);
                    current_ptr = &current->right;
                }

                if (!*current_ptr) {
                    *current_ptr = node;
                    node->left = NULL;
                    node->right = NULL;
                    node->balance_factor = 0;
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

static size_t max_height(size_t num_nodes);

/**
 *  Removes the node that compares equal to a key.
 *
 *  @param self Must not be NULL.
 *  @param comparator Must not be NULL. Must form the same total
 *                    ordering over the contained elements as the one
 *                    formed by the comparator passed to AvlMap_new.
 *                    Will be invoked by comparator(key, node, arg).
 *  @returns The node that compared equal to key, if there was one.
 */
AvlNode* AvlMap_remove(AvlMap *self, const void *key, AvlHetComparator comparator, void *arg) {
    NodeStack nodes;
    unsigned long is_left_flags_buf[IS_LEFT_FLAGS_BUF_SZ];
    BitStack is_left_flags;
    size_t current_depth = 0;
    AvlNode **current_ptr;
    AvlNode *to_remove;

    assert(self);

    NodeStack_with_capacity(&nodes, max_height(self->len) + 1);
    BitStack_from_adopted_slice(&is_left_flags, is_left_flags_buf, IS_LEFT_FLAGS_BUF_SZ);
    for (current_ptr = &self->root; *current_ptr; ++current_depth) {
        AvlNode *const current = *current_ptr;
        const int ordering = comparator(key, current, arg);

        NodeStack_push(&nodes, current);

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

            return 0; /* no such node... */
        }
    }

    to_remove = *current_ptr;
    remove_node(self, current_ptr, &nodes, &is_left_flags);
    --self->len;

    BitStack_drop(&is_left_flags);
    NodeStack_drop(&nodes);

    return to_remove;
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

    node->balance_factor = 0;

    assert(!node->left);
    assert(!node->right);

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
    assert(NodeStack_get(nodes, -1) == node);
    swap_idx = NodeStack_push(nodes, *successor_ptr) - 2;

    while ((*successor_ptr)->left) {
        successor_ptr = &(*successor_ptr)->left;
        BitStack_push_set(is_left_flags);
        NodeStack_push(nodes, *successor_ptr);
    }

    successor = *successor_ptr;

    assert(successor);
    assert(!successor->left);

    if (successor != node->right) {
        *successor_ptr = successor->right;
        successor->right = node->right;
    }

    successor->left = node->left;
    successor->balance_factor = node->balance_factor;

    node->right = NULL;
    node->left = NULL;

    *NodeStack_get_mut(nodes, (ptrdiff_t) swap_idx) = successor;

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
            AvlNode *const parent = NodeStack_get(nodes, -1);
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
                AvlNode *const middle_or_bottom = node->right;
                assert(middle_or_bottom);

                if (middle_or_bottom->balance_factor == -1) {
                    AvlNode *const middle = middle_or_bottom;
                    AvlNode *const bottom = middle->left;

                    assert(bottom);

                    node->right = rotate_right_unchecked(middle, bottom);
                    *parent_ptr = rotate_left_unchecked(node, bottom);

                    if (bottom->balance_factor == 1) {
                        node->balance_factor = -1;
                        middle->balance_factor = 0;
                    } else if (bottom->balance_factor == 0) {
                        node->balance_factor = 0;
                        middle->balance_factor = 0;
                    } else {
                        assert(bottom->balance_factor == -1);

                        node->balance_factor = 0;
                        middle->balance_factor = 1;
                    }

                    bottom->balance_factor = 0;
                } else {
                    AvlNode *const bottom = middle_or_bottom;

                    *parent_ptr = rotate_left_unchecked(node, bottom);

                    if (bottom->balance_factor == 0) {
                        bottom->balance_factor = -1;
                        node->balance_factor = 1;

                        break;
                    } else {
                        assert(bottom->balance_factor == 1);

                        bottom->balance_factor = 0;
                        node->balance_factor = 0;
                    }
                }
            }
        } else if (!is_left) {
            --node->balance_factor;

            if (node->balance_factor == -1) {
                return;
            } else if (node->balance_factor == -2) {
                AvlNode *const middle_or_bottom = node->left;
                assert(middle_or_bottom);

                if (middle_or_bottom->balance_factor == 1) {
                    AvlNode *const middle = middle_or_bottom;
                    AvlNode *const bottom = middle->right;

                    assert(bottom);

                    node->left = rotate_left_unchecked(middle, bottom);
                    *parent_ptr = rotate_right_unchecked(node, bottom);

                    if (bottom->balance_factor == -1) {
                        node->balance_factor = 1;
                        middle->balance_factor = 0;
                    } else if (bottom->balance_factor == 0) {
                        node->balance_factor = 0;
                        middle->balance_factor = 0;
                    } else {
                        assert(bottom->balance_factor == 1);

                        node->balance_factor = 0;
                        middle->balance_factor = -1;
                    }

                    bottom->balance_factor = 0;
                } else {
                    AvlNode *const bottom = middle_or_bottom;

                    *parent_ptr = rotate_right_unchecked(node, bottom);

                    if (bottom->balance_factor == 0) {
                        bottom->balance_factor = 1;
                        node->balance_factor = -1;

                        break;
                    } else {
                        assert(bottom->balance_factor == -1);

                        bottom->balance_factor = 0;
                        node->balance_factor = 0;
                    }
                }
            }
        }
    }
}

static double log2(double x);

static size_t max_height(size_t num_nodes) {
    return (size_t) ceil(1.44 * log2((double) num_nodes + 1.065) - 0.328);
}

static double log2(double x) {
    return log(x) / log(2.0);
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

    current = self->root;
    while (current) {
        AvlNode *next;

        while (current->left) {
            current = rotate_right_unchecked(current, current->left);
        }

        next = current->right;
        self->deleter(current, self->deleter_arg);
        current = next;
    }

    self->len = 0;
    self->root = NULL;
}
