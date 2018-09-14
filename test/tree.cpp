// BSD 3-Clause License
//
// Copyright (c) 2018, Gregory Meyer
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS AS IS
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#include <avl/tree.h>

#include <cstring>
#include <cstddef>

#include <sstream>

#include <catch.hpp>

static int strcmp_wrapper(const void *lhs, const void *rhs) {
    return std::strcmp(reinterpret_cast<const char*>(lhs), reinterpret_cast<const char*>(rhs));
}

static void traverse(std::ostream &os, const char *key) {
    os << key << '\n';
}

static void traverse_wrapper(void *context, const void *key, const void*) {
    traverse(*reinterpret_cast<std::ostream*>(context), reinterpret_cast<const char*>(key));
}

TEST_CASE("Tree initialization") {
    Tree tree;
    REQUIRE(Tree_init(&tree, &strcmp_wrapper) == TREE_SUCCESS);

    std::size_t size;
    REQUIRE(Tree_size(&tree, &size) == TREE_SUCCESS);
    REQUIRE(size == 0);


    REQUIRE(Tree_insert(&tree, "foo", nullptr) == TREE_SUCCESS);
    REQUIRE(Tree_size(&tree, &size) == TREE_SUCCESS);
    REQUIRE(size == 1);

    REQUIRE(Tree_insert(&tree, "bar", nullptr) == TREE_SUCCESS);
    REQUIRE(Tree_size(&tree, &size) == TREE_SUCCESS);
    REQUIRE(size == 2);

    REQUIRE(Tree_insert(&tree, "baz", nullptr) == TREE_SUCCESS);
    REQUIRE(Tree_size(&tree, &size) == TREE_SUCCESS);
    REQUIRE(size == 3);

    REQUIRE(Tree_insert(&tree, "qux", nullptr) == TREE_SUCCESS);
    REQUIRE(Tree_size(&tree, &size) == TREE_SUCCESS);
    REQUIRE(size == 4);


    const void *value;
    REQUIRE(Tree_find(&tree, "foo", &value) == TREE_SUCCESS);
    REQUIRE(!value);

    REQUIRE(Tree_find(&tree, "bar", &value) == TREE_SUCCESS);
    REQUIRE(!value);

    REQUIRE(Tree_find(&tree, "baz", &value) == TREE_SUCCESS);
    REQUIRE(!value);

    REQUIRE(Tree_find(&tree, "qux", &value) == TREE_SUCCESS);
    REQUIRE(!value);


    REQUIRE(Tree_insert(&tree, "foo", nullptr) == TREE_DUPLICATE_KEY);
    REQUIRE(Tree_size(&tree, &size) == TREE_SUCCESS);
    REQUIRE(size == 4);

    REQUIRE(Tree_insert(&tree, "bar", nullptr) == TREE_DUPLICATE_KEY);
    REQUIRE(Tree_size(&tree, &size) == TREE_SUCCESS);
    REQUIRE(size == 4);

    REQUIRE(Tree_insert(&tree, "baz", nullptr) == TREE_DUPLICATE_KEY);
    REQUIRE(Tree_size(&tree, &size) == TREE_SUCCESS);
    REQUIRE(size == 4);

    REQUIRE(Tree_insert(&tree, "qux", nullptr) == TREE_DUPLICATE_KEY);
    REQUIRE(Tree_size(&tree, &size) == TREE_SUCCESS);
    REQUIRE(size == 4);

    std::ostringstream oss;
    REQUIRE(Tree_traverse(&tree, &traverse_wrapper, &oss) == TREE_SUCCESS);
    REQUIRE(oss.str() == "bar\nbaz\nfoo\nqux\n");


    REQUIRE(Tree_clear(&tree) == TREE_SUCCESS);
    REQUIRE(Tree_size(&tree, &size) == TREE_SUCCESS);
    REQUIRE(size == 0);

    REQUIRE(Tree_find(&tree, "foo", &value) == TREE_NO_SUCH_KEY);
    REQUIRE(Tree_find(&tree, "bar", &value) == TREE_NO_SUCH_KEY);
    REQUIRE(Tree_find(&tree, "baz", &value) == TREE_NO_SUCH_KEY);
    REQUIRE(Tree_find(&tree, "qux", &value) == TREE_NO_SUCH_KEY);

    REQUIRE(Tree_destroy(&tree) == TREE_SUCCESS);
}
