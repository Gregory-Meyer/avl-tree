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
                 AvlDeleter deleter, void *deleter_arg);

/**
 *  Destroys an AvlMap, freeing all resources used.
 *
 *  @param self Must not be NULL. Must have been initialized by
 *              AvlMap_init and must not have been destroyed yet by
 *              AvlMap_destroy.
 */
void AvlMap_destroy(AvlMap *self);

/**
 *  Empties out an AvlMap, freeing all resources used.
 *
 *  @param self Must not be NULL. Must have been initialized by
 *              AvlMap_init.
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
