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

#ifndef BLOODHOUND_IMPL_MEM_H
#define BLOODHOUND_IMPL_MEM_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  Allocates uninitialized memory.
 *
 *  Uses malloc() internally. If malloc() returns NULL, a message is
 *  printed to stderr and abort() is called.
 *
 *  @param size The minimum number of bytes that the allocated memory
 *              should be able to hold.
 *  @returns A pointer to uninitialized memory at least size bytes
 *           long.
 */
void* checked_malloc(size_t size);

/**
 *  Allocates zero initialized memory.
 *
 *  Uses calloc() internally. If calloc() returns NULL, a message is
 *  printed to stderr and abort() is called.
 *
 *  @param num_objects The number of objects that the allocated memory
 *                     should be able to hold.
 *  @param object_size The length of each object in bytes.
 *  @returns A pointer to zero initialized memory.
 */
void* checked_calloc(size_t num_objects, size_t object_size);

/**
 *  Reallocates memory.
 *
 *  Uses realloc() internally. If realloc() returns NULL, a message is
 *  printed to stderr and abort() is called.
 *
 *  @param ptr A pointer to memory allocated by malloc(), calloc(), or
 *             realloc(). If NULL, this call is functionally equivalent
 *             to checked_malloc(new_size).
 *  @param new_size The minimum number of bytes that the allocated
 *                  memory should be able to hold.
 *  @returns A pointer to memory at least new_size bytes long. If
 *           new_size is greater the size ptr was allocated with, the
 *           first size bytes will be copied from ptr into the new
 *           memory and the remaining bytes are uninitialized.
 *           Otherwise, the first new_size bytes are copied and all
 *           memory is initialized.
 */
void* checked_realloc(void *ptr, size_t new_size);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
