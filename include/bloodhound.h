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

#ifndef BLOODHOUND_H
#define BLOODHOUND_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct AvlTree AvlTree;
typedef struct AvlNode AvlNode;

/* int compare(const AvlNode *lhs, const AvlNode *rhs, void *arg); */
typedef int (*AvlComparator)(const AvlNode*, const AvlNode*, void*);

/* int compare(const void *lhs, const AvlNode *rhs, void *arg); */
typedef int (*AvlHetComparator)(const void*, const AvlNode*, void*);

/* void traverse(void *context, const AvlNode *node); */
typedef void (*AvlTraverseCb)(void*, const AvlNode*);

/* void traverse_mut(void *context, AvlNode *node); */
typedef void (*AvlTraverseMutCb)(void*, AvlNode*);

/* void delete(AvlNode *node, void *arg); */
typedef void (*AvlDeleter)(AvlNode*, void*);

/**
 *  Initializes an empty AvlTree.
 *
 *  @param self Must not be NULL. Must not be initialized.
 *  @param compare Must not be NULL. Will be invoked to compare nodes
 *                 by compare(lhs, rhs, compare_arg). Return values
 *                 should have the same meaning as strcmp and should
 *                 form a total ordering over the set of nodes.
 *  @param deleter Must not be NULL. Will be used to free nodes when
 *                 they are no longer usable by the tree as if by
 *                 deleter(node, deleter_arg).
 */
void AvlTree_new(AvlTree *self, AvlComparator compare, void *compare_arg,
                 AvlDeleter deleter, void *deleter_arg);

/**
 *  Drops an AvlTree, removing all members.
 *
 *  Equivalent to AvlTree_clear.
 *
 *  @param self Must not be NULL. Must be initialized.
 */
void AvlTree_drop(AvlTree *self);

/**
 *  @param self Must not be NULL. Must be initialized.
 *  @param compare Must not be NULL. Must form the same total ordering
 *                 over the contained elements as the one formed by one
 *                 passed to AvlTree_new. Will be invoked by
 *                 compare(key, node, arg).
 *  @returns A pointer to the node that compares equal to key, if
 *           there is one.
 */
const AvlNode* AvlTree_get(const AvlTree *self, const void *key,
                           AvlHetComparator compare, void *arg);

/**
 *  @param self Must not be NULL. Must be initialized.
 *  @param compare Must not be NULL. Must form the same total ordering
 *                 over the contained elements as the one formed by one
 *                 passed to AvlTree_new. Will be invoked by
 *                 compare(key, node, arg).
 *  @returns A mutable pointer to the node that compares equal to key,
 *           if there is one.
 */
AvlNode* AvlTree_get_mut(AvlTree *self, const void *key, AvlHetComparator compare, void *arg);

/**
 *  Inserts an element into an AvlTree.
 *
 *  @param self Must not be NULL. Must be initialized.
 *  @param node Must not be NULL.
 *  @returns The previous element that compares equal to node, if there
 *           was one.
 */
AvlNode* AvlTree_insert(AvlTree *self, AvlNode *node);

/**
 *  Removes the node that compares equal to a key.
 *
 *  @param self Must not be NULL.
 *  @param compare Must not be NULL. Must form the same total
 *                 ordering over the set of nodes as the one
 *                 passed to AvlTree_new. Will be invoked to compare
 *                 the key to nodes by compare(key, node, arg).
 *  @returns The node that compared equal to key, if there was one.
 */
AvlNode* AvlTree_remove(AvlTree *self, const void *key, AvlHetComparator compare, void *arg);

/**
 *  Clears the tree, removing all members.
 *
 *  @param self Must not be NULL. Must be initialized.
 */
void AvlTree_clear(AvlTree *self);

struct AvlTree {
    AvlNode *root;
    size_t len;
    AvlComparator compare;
    void *compare_arg;
    AvlDeleter deleter;
    void *deleter_arg;
};

struct AvlNode {
    AvlNode *left;
    AvlNode *right;
    signed char balance_factor; /* one of {-2, -1, 0, 1, -2} */
};

#ifdef __cplusplus
} // extern "C"
#endif

#endif
