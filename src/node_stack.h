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

#ifndef AVLBST_IMPL_NODE_STACK_H
#define AVLBST_IMPL_NODE_STACK_H

#include <avlbst.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  LIFO queue of AvlNodes.
 *
 *  NodeStack does not own the AvlNodes it holds pointers to, so users
 *  should take care to ensure that the owner of these nodes outlives
 *  this NodeStack.
 */
typedef struct NodeStack NodeStack;

/**
 *  Initializes an empty NodeStack.
 *
 *  @param self Must not be NULL. Must not be initialized.
 */
void NodeStack_init(NodeStack *self);

/**
 *  Destroys a NodeStack, deallocating all owned resources.
 *
 *  @param self Must not be NULL. Must not be initialized.
 */
void NodeStack_destroy(NodeStack *self);

/**
 *  Pushes an AvlNode to the top of this NodeStack.
 *
 *  If not enough space is available for this NodeStack, realloc() is
 *  called to increase the capacity of the NodeStack by 1.5 - if there
 *  is no capacity, malloc() is called to initialize the NodeStack with
 *  space for 8 node pointers.
 *
 *  @param self Must not be NULL. Must be initialized.
 *  @param node Will be yielded as the next result of pop().
 */
void NodeStack_push(NodeStack *self, AvlNode *node);

/**
 *  Removes an AvlNode from the top of this NodeStack, if there is one.
 *
 *  @param self Must not be NULL. Must be initialized.
 *  @returns The AvlNode that was at the top of this NodeStack. If this
 *           NodeStack was empty, NULL.
 */
AvlNode* NodeStack_pop(NodeStack *self);

/**
 *  Retrieves the AvlNode that is index elements from the top of this
 *  NodeStack, if there is one.
 *
 *  @param self Must not be NULL. Must be initialized.
 *  @param index The index of the element to fetch, where 0 is the top.
 *  @returns The AvlNode that was index elements from the top of this
 *           NodeStack. If no such element exists, NULL.
 */
AvlNode* NodeStack_get(const NodeStack *self, size_t index);

/**
 *  LIFO queue of AvlNodes.
 *
 *  NodeStack does not own the AvlNodes it holds pointers to, so users
 *  should take care to ensure that the owner of these nodes outlives
 *  this NodeStack.
 */
struct NodeStack {
    AvlNode **data;
    size_t len;
    size_t capacity;
};

#ifdef __cplusplus
} // extern "C"
#endif

#endif
