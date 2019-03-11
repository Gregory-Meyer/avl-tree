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

#include "node_stack.h"

#include "mem.h"

#include <assert.h>
#include <stdlib.h>

/**
 *  Initializes an empty NodeStack.
 *
 *  @param self Must not be NULL. Must not be initialized.
 */
void NodeStack_new(NodeStack *self) {
    assert(self);

    self->data = NULL;
    self->len = 0;
    self->capacity = 0;
}

/**
 *  Initializes an empty NodeStack with space for at least size
 *  elements.
 *
 *  @param self Must not be NULL. Must not be initialized.
 */
void NodeStack_with_capacity(NodeStack *self, size_t size) {
    assert(self);

    if (size == 0) {
        NodeStack_new(self);

        return;
    }

    self->data = checked_malloc(sizeof(AvlNode*) * size);
    self->len = 0;
    self->capacity = size;
}

/**
 *  Drops a NodeStack, deallocating all owned resources.
 *
 *  @param self Must not be NULL. Must not be initialized.
 */
void NodeStack_drop(NodeStack *self) {
    free(self->data);
}

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
void NodeStack_push(NodeStack *self, AvlNode *node) {
    assert(self);

    if (self->capacity == self->len) {
        if (!self->data) {
            assert(self->len == 0);

            self->data = checked_malloc(sizeof(AvlNode*) * 8);
            self->capacity = 8;
        } else {
            assert(self->capacity >= 8);

            self->capacity *= 3;
            self->capacity /= 2;

            self->data = checked_realloc(self->data, sizeof(AvlNode*) * self->capacity);
        }
    }

    self->data[self->len] = node;
    ++self->len;
}

/**
 *  Removes an AvlNode from the top of this NodeStack, if there is one.
 *
 *  @param self Must not be NULL. Must be initialized.
 *  @returns The AvlNode that was at the top of this NodeStack. If this
 *           NodeStack was empty, NULL.
 */
AvlNode* NodeStack_pop(NodeStack *self) {
    assert(self);

    if (self->len == 0) {
        return NULL;
    }

    --self->len;

    return self->data[self->len];
}

/**
 *  Retrieves the AvlNode that is index elements from the top of this
 *  NodeStack, if there is one.
 *
 *  @param self Must not be NULL. Must be initialized.
 *  @param index The index of the element to fetch, where 0 is the top.
 *  @returns The AvlNode that was index elements from the top of this
 *           NodeStack. If no such element exists, NULL.
 */
AvlNode* NodeStack_get(const NodeStack *self, size_t index) {
    assert(self);

    if (self->len <= index) {
        return NULL;
    }

    return self->data[self->len - index - 1];
}
