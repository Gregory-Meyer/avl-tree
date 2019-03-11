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

#include "mem.h"

#include <stdlib.h>
#include <stdio.h>

void* checked_malloc(size_t size) {
    void *const ptr = malloc(size);

    if (!ptr) {
        fprintf(stderr, "libavlbst: checked_malloc(): malloc() returned NULL\n");
        abort();
    }

    return ptr;
}

void* checked_calloc(size_t num_objects, size_t object_size) {
    void *const ptr = calloc(num_objects, object_size);

    if (!ptr) {
        fprintf(stderr, "libavlbst: checked_calloc(): calloc() returned NULL\n");
        abort();
    }

    return ptr;
}

void* checked_realloc(void *ptr, size_t new_size) {
    ptr = realloc(ptr, new_size);

    if (!ptr) {
        fprintf(stderr, "libavlbst: checked_realloc(): realloc() returned NULL\n");
        abort();
    }

    return ptr;
}
