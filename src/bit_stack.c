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

#include "bit_stack.h"

#include "mem.h"

#include <assert.h>
#include <limits.h>
#include <stdlib.h>

#define BITS_PER_WORD (CHAR_BIT * sizeof(unsigned long))

/**
 *  Initializes an empty BitStack.
 *
 *  @param self Must not be NULL. Must not be initialized.
 */
void BitStack_new(BitStack *self) {
    assert(self);

    self->data = NULL;
    self->len = 0;
    self->capacity = 0;
}

static size_t div_towards_inf(size_t x, size_t y) {
    const size_t result = x / y;
    const size_t remainder = x % y;

    if (remainder != 0) {
        return result + 1;
    } else {
        return result;
    }
}

/**
 *  Initializes an empty BitStack with space for at least capacity
 *  bits.
 *
 *  @param self Must not be NULL. Must not be initialized.
 */
void BitStack_with_capacity(BitStack *self, size_t capacity) {
    size_t num_words;

    assert(self);

    num_words = div_towards_inf(capacity, BITS_PER_WORD);

    self->data = checked_malloc(sizeof(unsigned long) * num_words);
    self->len = 0;
    self->capacity = num_words * BITS_PER_WORD;
}

/**
 *  Drops a BitStack, deallocating all owned resources.
 *
 *  @param self Must not be NULL. Must be initialized.
 */
void BitStack_drop(BitStack *self) {
    assert(self);

    free(self->data);
}

static void grow_if_full(BitStack *self);

/**
 *  Pushes a set bit to the top of this BitStack.
 *
 *  @param self Must not be NULL. Must be initialized.
 *  @returns The new length of this BitStack.
 */
size_t BitStack_push_set(BitStack *self) {
    size_t word_idx;
    size_t bit_idx;

    assert(self);

    grow_if_full(self);

    word_idx = self->len / BITS_PER_WORD;
    bit_idx = self->len % BITS_PER_WORD;

    self->data[word_idx] |= (1ul << bit_idx);
    ++self->len;

    return self->len;
}

/**
 *  Pushes an unset bit to the top of this BitStack.
 *
 *  @param self Must not be NULL. Must be initialized.
 *  @returns The new length of this BitStack.
 */
size_t BitStack_push_clear(BitStack *self) {
    size_t word_idx;
    size_t bit_idx;

    assert(self);

    grow_if_full(self);

    word_idx = self->len / BITS_PER_WORD;
    bit_idx = self->len % BITS_PER_WORD;

    self->data[word_idx] &= ~(1ul << bit_idx);
    ++self->len;

    return self->len;
}

static void grow_if_full(BitStack *self) {
    assert(self);

    if (self->len < self->capacity) {
        return;
    }

    if (!self->data) {
        self->data = checked_malloc(sizeof(unsigned long) * 4);
        self->capacity = 4 * BITS_PER_WORD;
    } else {
        const size_t new_word_count = (self->capacity / BITS_PER_WORD + 1) * 3 / 2;

        self->data = checked_realloc(self->data, sizeof(unsigned long) * new_word_count);
        self->capacity = new_word_count * BITS_PER_WORD;
    }
}

/**
 *  Removes a bit from the top of this BitStack, if there is one.
 *
 *  @param self Must not be NULL. Must be initialized.
 *  @returns 1 if the top bit was set, 0 if it was not set, or -1 if
 *           this BitStack is empty.
 */
int BitStack_pop(BitStack *self) {
    assert(self);

    if (self->len == 0) {
        return -1;
    } else {
        size_t word_idx;
        size_t bit_idx;

        --self->len;

        word_idx = self->len / BITS_PER_WORD;
        bit_idx = self->len % BITS_PER_WORD;

        return (self->data[word_idx] & (1ul << bit_idx)) != 0;
    }
}

/**
 *  Accesses a bit indexed from the top of this BitStack.
 *
 *  @param self Must not be NULL. Must be initialized.
 *  @returns 1 if the bit index elements from the top of the stack is
 *           set, 0 if it is not set, or -1 if there is no element
 *           index bits from the top.
 */
int BitStack_get(const BitStack *self, size_t index) {
    assert(self);

    if (self->len <= index) {
        return -1;
    } else {
        const size_t word_idx = self->len / BITS_PER_WORD;
        const size_t bit_idx = self->len % BITS_PER_WORD;

        return (self->data[word_idx] & (1ul << bit_idx)) != 0;
    }
}

/** @returns The number of bits contained in this BitStack. */
size_t BitStack_len(const BitStack *self) {
    assert(self);

    return self->len;
}
