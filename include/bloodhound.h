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

/**
 *  @file bloodhound.h
 *
 *  C89 implementation of an AVL self-balancing binary search tree with
 *  intrusive nodes.
 */

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  AVL self-balancing binary search tree.
 *
 *  AVL trees maintain a strict "AVL condition": for each node,
 *  the heights of its subtrees never differ by more than one. This
 *  condition guarantees that the tree's height is upper-bounded by
 *  1.44 log2(n + 1.065) - 0.328, where n is the number of nodes in the
 *  tree. As such, insertion, removal, and searching are guaranteed to
 *  run in O(log n) worst case time.
 */
typedef struct AvlTree AvlTree;

/**
 *  Intrusive AVL tree node.
 *
 *  Users should create custom structs that have AvlNode as a member,
 *  such as by composition or inheritance (in C++). Although it may be
 *  tempting, users should not modify the contents of AvlNode in their
 *  own types. For example, the following is a node that holds a
 *  const char* key and an int value:
 *
 *  @code{.c}
 *  typedef struct Node {
 *      AvlNode node;
 *      const char *key;
 *      int value;
 *  } Node;
 *
 *  AvlTree map;
 *  Node n = {{NULL, NULL, 0}, "Hello, world!", 42};
 *  Node *const previous = (Node*) AvlTree_insert(&map, &n.node);
 *  @endcode
 *
 *  In C, I recommend placing the AvlNode as the first member of your
 *  struct to ensure that casting between your own node types and
 *  AvlNode is valid. You can put the AvlNode member at a different
 *  spot if you want to, but you'll have to use offsetof() to convert
 *  between pointer types.
 *
 *  This is a similar example using inheritance in C++ to allow for
 *  easy use of static_cast:
 *
 *  @code{.cpp}
 *  struct Node : AvlNode {
 *      Node(std::string k, int v) : key(std::move(k)), value(v) { }
 *
 *      std::string key;
 *      int value;
 *  }
 *
 *  AvlTree map;
 *  Node n("Hello, world!", 42);
 *  const auto previous = static_cast<Node*>(AvlTree_insert(&map, &n));
 *  @endcode
 */
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

/**
 *  AVL self-balancing binary search tree.
 *
 *  AVL trees maintain a strict "AVL condition": for each node,
 *  the heights of its subtrees never differ by more than one. This
 *  condition guarantees that the tree's height is upper-bounded by
 *  1.44 log2(n + 1.065) - 0.328, where n is the number of nodes in the
 *  tree. As such, insertion, removal, and searching are guaranteed to
 *  run in O(log n) worst case time.
 */
struct AvlTree {
    AvlNode *root;
    size_t len;
    AvlComparator compare;
    void *compare_arg;
    AvlDeleter deleter;
    void *deleter_arg;
};

/**
 *  Intrusive AVL tree node.
 *
 *  Users should create custom structs that have AvlNode as a member,
 *  such as by composition or inheritance (in C++). Although it may be
 *  tempting, users should not modify the contents of AvlNode in their
 *  own types. For example, the following is a node that holds a
 *  const char* key and an int value:
 *
 *  @code{.c}
 *  typedef struct Node {
 *      AvlNode node;
 *      const char *key;
 *      int value;
 *  } Node;
 *
 *  AvlTree map;
 *  Node n = {{NULL, NULL, 0}, "Hello, world!", 42};
 *  Node *const previous = (Node*) AvlTree_insert(&map, &n.node);
 *  @endcode
 *
 *  In C, I recommend placing the AvlNode as the first member of your
 *  struct to ensure that casting between your own node types and
 *  AvlNode is valid. You can put the AvlNode member at a different
 *  spot if you want to, but you'll have to use offsetof() to convert
 *  between pointer types.
 *
 *  This is a similar example using inheritance in C++ to allow for
 *  easy use of static_cast:
 *
 *  @code{.cpp}
 *  struct Node : AvlNode {
 *      Node(std::string k, int v) : key(std::move(k)), value(v) { }
 *
 *      std::string key;
 *      int value;
 *  }
 *
 *  AvlTree map;
 *  Node n("Hello, world!", 42);
 *  const auto previous = static_cast<Node*>(AvlTree_insert(&map, &n));
 *  @endcode
 */
struct AvlNode {
    AvlNode *left;
    AvlNode *right;
    signed char balance_factor; /* one of {-2, -1, 0, 1, -2} */
};

#ifdef __cplusplus
} // extern "C"
#endif

#endif
