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

#ifndef BLOODHOUND_IMPL_NODE_H
#define BLOODHOUND_IMPL_NODE_H

#include <bloodhound.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  Automatically selects a rotation to execute on a tree.
 *
 *  If root has a balance factor of 2 and its right child has a balance
 *  factor of 1, executes a left rotation around root. If root's right
 *  child has a balance factor of -1, executes a right-left rotation
 *  around root. If root has a balance factor of -2 and its left child
 *  has a balance factor of -1, right rotates around root. If its left
 *  child has a balance factor of 1, executes a left-right rotation
 *  around root.
 *
 *  @param root Must not be NULL. Must have a left or right child,
 *              depending on its balance factor, which may also then
 *              require a left or right child.
 *  @returns The new root of the tree.
 */
AvlNode* rotate(AvlNode *root);

/**
 *  Executes a left rotation around middle, then a right rotation
 *  around top.
 *
 *  @param top Must not be NULL. Must have a balance factor of -2. Must
 *             have middle as its left child.
 *  @param middle Must not be NULL. Must have a balance factor of 1.
 *                Must have bottom as its right child.
 *  @param bottom Must not be NULL.
 *  @returns bottom, the new root of the tree.
 */
AvlNode* rotate_leftright(AvlNode *top, AvlNode *middle, AvlNode *bottom);

/**
 *  Executes a right rotation around middle, then a left rotation
 *  around top.
 *
 *  @param top Must not be NULL. Must have a balance factor of 2. Must
 *             have middle as its right child.
 *  @param middle Must not be NULL. Must have a balance factor of -1.
 *                Must have bottom as its left child.
 *  @param bottom Must not be NULL.
 *  @returns bottom, the new root of the tree.
 */
AvlNode* rotate_rightleft(AvlNode *top, AvlNode *middle, AvlNode *bottom);

/**
 *  Executes a left rotation around top.
 *
 *  @param top Must not be NULL. Must have bottom as its right child.
 *             Must have a balance factor of 2.
 *  @param bottom Must not be NULL. Must have a balance factor of 1.
 *  @returns bottom, the new root of the tree.
 */
AvlNode* rotate_left(AvlNode *top, AvlNode *bottom);

/**
 *  Executes a right rotation around top.
 *
 *  @param top Must not be NULL. Must have bottom as its left child.
 *             Must have a balance factor of -2.
 *  @param bottom Must not be NULL. Must have a balance factor of -1.
 *  @returns bottom, the new root of the tree.
 */
AvlNode* rotate_right(AvlNode *top, AvlNode *bottom);

/**
 *  Executes a left rotation around top.
 *
 *  @param top Must not be NULL. Must have bottom as its right child.
 *  @param bottom Must not be NULL.
 *  @returns bottom, the new root of the tree.
 */
AvlNode* rotate_left_unchecked(AvlNode *top, AvlNode *bottom);

/**
 *  Executes a right rotation around top.
 *
 *  @param top Must not be NULL. Must have bottom as its left child.
 *  @param bottom Must not be NULL.
 *  @returns bottom, the new root of the tree.
 */
AvlNode* rotate_right_unchecked(AvlNode *top, AvlNode *bottom);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
