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

#include "node.h"

#include <assert.h>

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
AvlNode* rotate(AvlNode *root) {
    assert(root);

    if (root->balance_factor == -2) {
        AvlNode *const middle_or_bottom = root->left;
        assert(middle_or_bottom);

        if (middle_or_bottom->balance_factor == -1) {
            AvlNode *const bottom = middle_or_bottom;

            return rotate_right(root, bottom);
        } else { /* middle_or_bottom->balance_factor == 1 */
            AvlNode *const middle = middle_or_bottom;
            AvlNode *const bottom = middle->right;

            assert(middle->balance_factor == 1);
            assert(bottom);

            return rotate_leftright(root, middle, bottom);
        }
    } else if (root->balance_factor == 2) {
        AvlNode *const middle_or_bottom = root->right;
        assert(middle_or_bottom);

        if (middle_or_bottom->balance_factor == 1) {
            AvlNode *const bottom = middle_or_bottom;

            return rotate_left(root, bottom);
        } else { /* middle_or_bottom->balance_factor == -1 */
            AvlNode *const middle = middle_or_bottom;
            AvlNode *const bottom = middle->left;

            assert(middle->balance_factor == -1);
            assert(bottom);

            return rotate_leftright(root, middle, bottom);
        }
    } else {
        return root;
    }
}

static AvlNode* do_rotate_right(AvlNode *top, AvlNode *bottom);

static AvlNode* do_rotate_left(AvlNode *top, AvlNode *bottom);

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
AvlNode* rotate_leftright(AvlNode *top, AvlNode *middle, AvlNode *bottom) {
    assert(top);
    assert(top->balance_factor == -2);
    assert(top->left == middle);
    assert(middle);
    assert(middle->balance_factor == 1);
    assert(middle->right = bottom);
    assert(bottom);

    top->right = do_rotate_right(middle, bottom);
    do_rotate_left(top, bottom);

    if (bottom->balance_factor == 1) {
        top->balance_factor = -1;
        middle->balance_factor = 0;
    } else if (bottom->balance_factor == 0) {
        top->balance_factor = 0;
        middle->balance_factor = 0;
    } else {
        assert(bottom->balance_factor == -1);

        top->balance_factor = 0;
        middle->balance_factor = 1;
    }

    bottom->balance_factor = 0;

    return bottom;
}

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
AvlNode* rotate_rightleft(AvlNode *top, AvlNode *middle, AvlNode *bottom) {
    assert(top);
    assert(top->balance_factor == 2);
    assert(top->right == middle);
    assert(middle);
    assert(middle->balance_factor == -1);
    assert(middle->left = bottom);
    assert(bottom);

    top->left = do_rotate_left(middle, bottom);
    do_rotate_right(top, bottom);

    if (bottom->balance_factor == -1) {
        top->balance_factor = 1;
        middle->balance_factor = 0;
    } else if (bottom->balance_factor == 0) {
        top->balance_factor = 0;
        middle->balance_factor = 0;
    } else {
        assert(bottom->balance_factor == 1);

        top->balance_factor = 0;
        middle->balance_factor = -1;
    }

    bottom->balance_factor = 0;

    return bottom;
}

/**
 *  Executes a left rotation around top.
 *
 *  @param top Must not be NULL. Must have bottom as its right child.
 *             Must have a balance factor of 2.
 *  @param bottom Must not be NULL. Must have a balance factor of 1.
 *  @returns bottom, the new root of the tree.
 */
AvlNode* rotate_left(AvlNode *top, AvlNode *bottom) {
    assert(top);
    assert(top->right == bottom);
    assert(top->balance_factor == 2);
    assert(bottom);
    assert(bottom->balance_factor == 1);

    do_rotate_left(top, bottom);

    top->balance_factor = 0;
    bottom->balance_factor = 0;

    return bottom;
}

/**
 *  Executes a right rotation around top.
 *
 *  @param top Must not be NULL. Must have bottom as its left child.
 *             Must have a balance factor of -2.
 *  @param bottom Must not be NULL. Must have a balance factor of -1.
 *  @returns bottom, the new root of the tree.
 */
AvlNode* rotate_right(AvlNode *top, AvlNode *bottom) {
    assert(top);
    assert(top->left == bottom);
    assert(top->balance_factor == -2);
    assert(bottom);
    assert(bottom->balance_factor == -1);

    do_rotate_right(top, bottom);

    top->balance_factor = 0;
    bottom->balance_factor = 0;

    return bottom;
}

static AvlNode* do_rotate_left(AvlNode *top, AvlNode *bottom) {
    assert(top);
    assert(bottom);
    assert(top->right == bottom);

    top->right = bottom->left;
    bottom->left = top;

    return bottom;
}

static AvlNode* do_rotate_right(AvlNode *top, AvlNode *bottom) {
    assert(top);
    assert(bottom);
    assert(top->left == bottom);

    top->left = bottom->right;
    bottom->right = top;

    return bottom;
}
