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

#ifndef BLOODHOUND_IMPL_BIT_STACK_H
#define BLOODHOUND_IMPL_BIT_STACK_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/** LIFO queue of bits. */
typedef struct BitStack BitStack;

/**
 *  Initializes an empty BitStack.
 *
 *  @param self Must not be NULL. Must not be initialized.
 */
void BitStack_new(BitStack *self);

/**
 *  Initializes an empty BitStack with space for at least capacity
 *  bits.
 *
 *  @param self Must not be NULL. Must not be initialized.
 */
void BitStack_with_capacity(BitStack *self, size_t capacity);

/**
 *  Drops a BitStack, deallocating all owned resources.
 *
 *  @param self Must not be NULL. Must be initialized.
 */
void BitStack_drop(BitStack *self);

/**
 *  Pushes a set bit to the top of this BitStack.
 *
 *  @param self Must not be NULL. Must be initialized.
 *  @returns The new length of this BitStack.
 */
size_t BitStack_push_set(BitStack *self);

/**
 *  Pushes an unset bit to the top of this BitStack.
 *
 *  @param self Must not be NULL. Must be initialized.
 *  @returns The new length of this BitStack.
 */
size_t BitStack_push_clear(BitStack *self);

/**
 *  Removes a bit from the top of this BitStack, if there is one.
 *
 *  @param self Must not be NULL. Must be initialized.
 *  @returns 1 if the top bit was set, 0 if it was not set, or -1 if
 *           this BitStack is empty.
 */
int BitStack_pop(BitStack *self);

/**
 *  Removes all bits from this BitStack.
 *
 *  @param self Must not be NULL. Must be initialized.
 */
void BitStack_clear(BitStack *self);

/**
 *  Accesses a bit indexed from the top of this BitStack.
 *
 *  @param self Must not be NULL. Must be initialized.
 *  @returns 1 if the bit index elements from the top of the stack is
 *           set, 0 if it is not set, or -1 if there is no element
 *           index bits from the top.
 */
int BitStack_get(const BitStack *self, size_t index);

/** @returns The number of bits contained in this BitStack. */
size_t BitStack_len(const BitStack *self);

/** LIFO queue of bits. */
struct BitStack {
    unsigned long *data;
    size_t len;
    size_t capacity;
};

#ifdef __cplusplus
} // extern "C"
#endif

#endif
