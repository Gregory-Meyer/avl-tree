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

typedef struct AvlMap AvlMap;
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
                AvlDeleter deleter, void *deleter_arg);

/**
 *  Drops an AvlMap, removing all members.
 *
 *  Equivalent to AvlMap_clear. Runs in O(1) stack frames and O(n) time
 *  complexity.
 *
 *  @param self Must not be NULL. Must be initialized.
 */
void AvlMap_drop(AvlMap *self);

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
                          void *arg);

/**
 *  @param self Must not be NULL. Must be initialized.
 *  @param comparator Must not be NULL. Must form the same total
 *                    ordering over the contained elements as the one
 *                    formed by the comparator passed to AvlMap_new.
 *                    Will be invoked by comparator(key, node, arg).
 *  @returns A mutable pointer to the node that compares equal to key,
 *           if there is one.
 */
AvlNode* AvlMap_get_mut(AvlMap *self, const void *key, AvlHetComparator comparator, void *arg);

/**
 *  Inserts an element into an AvlMap.
 *
 *  @param self Must not be NULL. Must be initialized.
 *  @param node Must not be NULL.
 *  @returns The previous element that compares equal to node, if there
 *           was one.
 */
AvlNode* AvlMap_insert(AvlMap *self, AvlNode *node);

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
AvlNode* AvlMap_remove(AvlMap *self, const void *key, AvlHetComparator comparator, void *arg);

/**
 *  Clears the map, removing all members.
 *
 *  Runs in O(1) stack frames and O(n) time complexity.
 *
 *  @param self Must not be NULL. Must be initialized.
 */
void AvlMap_clear(AvlMap *self);

struct AvlMap {
    AvlNode *root;
    size_t len;
    AvlComparator compare;
    void *compare_arg;
    AvlDeleter deleter;
    void *deleter_arg;
};

/** AVL Tree node. */
struct AvlNode {
    AvlNode *left;
    AvlNode *right;
    signed char balance_factor; /* on a 64-bit architecture this is <= 90 */
};

#ifdef __cplusplus
} // extern "C"
#endif

#endif
