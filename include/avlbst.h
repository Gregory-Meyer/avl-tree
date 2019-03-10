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

#ifndef AVLBST_H
#define AVLBST_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct AvlMap AvlMap;
typedef struct AvlNode AvlNode;

/* int compare(const void *lhs, const void *rhs, void *arg); */
typedef int (*AvlComparator)(const void*, const void*, void*);

/* void traverse(void *context, const void *key, const void *value); */
typedef void (*AvlTraverseCb)(void*, const void*, const void*);

/* void traverse_mut(void *context, const void *key, void *value); */
typedef void (*AvlTraverseMutCb)(void*, const void*, void*);

/* void delete(void *key, void *value, void *arg); */
typedef void (*AvlDeleter)(void*, void*, void*);

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
                 AvlDeleter deleter, void *deleter_arg);

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
void* AvlMap_insert(AvlMap *self, void *key, void *value);

/**
 *  Destroys an AvlMap, removing all members.
 *
 *  Equivalent to AvlMap_clear. Runs in O(1) stack frames and O(n) time
 *  complexity.
 *
 *  @param self Must not be NULL. Must be initialized.
 */
void AvlMap_destroy(AvlMap *self);

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

#ifdef __cplusplus
} // extern "C"
#endif

#endif
