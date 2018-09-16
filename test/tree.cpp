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
#include <string>
#include <vector>

#include <catch.hpp>

static int strcmp_wrapper(const void *lhs, const void *rhs) {
    return std::strcmp(reinterpret_cast<const char*>(lhs), reinterpret_cast<const char*>(rhs));
}

static constexpr intptr_t intptr_compare(std::intptr_t lhs, std::intptr_t rhs) {
    return lhs - rhs;
}

static int intptr_compare_wrapper(const void *lhs, const void *rhs) {
    const auto result = intptr_compare(*reinterpret_cast<const std::intptr_t*>(lhs),
                                       *reinterpret_cast<const std::intptr_t*>(rhs));

    if (result < 0) {
        return -1;
    } else if (result > 0) {
        return 1;
    }

    return 0;
}

static void traverse(std::ostream &os, const char *key) {
    os << key << '\n';
}

static void char_traverse_wrapper(void *context, const void *key, const void*) {
    traverse(*reinterpret_cast<std::ostream*>(context), reinterpret_cast<const char*>(key));
}

static void traverse(std::ostream &os, std::intptr_t key) {
    os << key << '\n';
}

static void intptr_traverse_wrapper(void *context, const void *key, const void*) {
    traverse(*reinterpret_cast<std::ostream*>(context), *reinterpret_cast<const intptr_t*>(key));
}

TEST_CASE("Tree initialization") {
    Tree tree;
    REQUIRE(Tree_init(&tree, &strcmp_wrapper) == TREE_SUCCESS);

    std::size_t size;
    REQUIRE(Tree_size(&tree, &size) == TREE_SUCCESS);
    REQUIRE(size == 0);

    REQUIRE(Tree_destroy(&tree) == TREE_SUCCESS);
}

TEST_CASE("Tree insertion") {
    Tree tree;
    REQUIRE(Tree_init(&tree, &strcmp_wrapper) == TREE_SUCCESS);

    std::size_t size = 0;
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

    REQUIRE(Tree_destroy(&tree) == TREE_SUCCESS);
}

TEST_CASE("Tree traversal") {
    Tree tree;
    REQUIRE(Tree_init(&tree, &strcmp_wrapper) == TREE_SUCCESS);

    REQUIRE(Tree_insert(&tree, "foo", nullptr) == TREE_SUCCESS);

    {
        std::ostringstream oss;
        REQUIRE(Tree_traverse(&tree, &char_traverse_wrapper, &oss) == TREE_SUCCESS);
        REQUIRE(oss.str() == "foo\n");
    }

    REQUIRE(Tree_insert(&tree, "bar", nullptr) == TREE_SUCCESS);

    {
        std::ostringstream oss;
        REQUIRE(Tree_traverse(&tree, &char_traverse_wrapper, &oss) == TREE_SUCCESS);
        REQUIRE(oss.str() == "bar\nfoo\n");
    }

    REQUIRE(Tree_insert(&tree, "baz", nullptr) == TREE_SUCCESS);

    {
        std::ostringstream oss;
        REQUIRE(Tree_traverse(&tree, &char_traverse_wrapper, &oss) == TREE_SUCCESS);
        REQUIRE(oss.str() == "bar\nbaz\nfoo\n");
    }

    REQUIRE(Tree_insert(&tree, "qux", nullptr) == TREE_SUCCESS);

    {
        std::ostringstream oss;
        REQUIRE(Tree_traverse(&tree, &char_traverse_wrapper, &oss) == TREE_SUCCESS);
        REQUIRE(oss.str() == "bar\nbaz\nfoo\nqux\n");
    }

    REQUIRE(Tree_destroy(&tree) == TREE_SUCCESS);
}

TEST_CASE("Tree traversal with intptr_t") {
    Tree tree;
    REQUIRE(Tree_init(&tree, &intptr_compare_wrapper) == TREE_SUCCESS);

    const std::vector<std::intptr_t> vec{ 3, 2, 1, 4, 5, 6, 7, 16, 15, 14 };

    for (const auto &key : vec) {
        REQUIRE(Tree_insert(&tree, reinterpret_cast<const void*>(&key), nullptr) == TREE_SUCCESS);
    }

    std::ostringstream oss;
    REQUIRE(Tree_traverse(&tree, &intptr_traverse_wrapper, &oss) == TREE_SUCCESS);
    REQUIRE(oss.str() == "1\n2\n3\n4\n5\n6\n7\n14\n15\n16\n");

    REQUIRE(Tree_destroy(&tree) == TREE_SUCCESS);
}

TEST_CASE("Tree erasure") {
    Tree tree;
    REQUIRE(Tree_init(&tree, &strcmp_wrapper) == TREE_SUCCESS);

    const std::vector<std::string> vec{ "foo", "bar", "baz", "qux" };

    for (const auto &key : vec) {
        REQUIRE(Tree_insert(&tree, reinterpret_cast<const void*>(key.c_str()), nullptr) == TREE_SUCCESS);
    }

    REQUIRE(Tree_erase(&tree, "foo") == TREE_SUCCESS);
    REQUIRE(Tree_erase(&tree, "foo") == TREE_NO_SUCH_KEY);

    {
        std::ostringstream oss;
        REQUIRE(Tree_traverse(&tree, &char_traverse_wrapper, &oss) == TREE_SUCCESS);
        REQUIRE(oss.str() == "bar\nbaz\nqux\n");
    }

    REQUIRE(Tree_erase(&tree, "bar") == TREE_SUCCESS);
    REQUIRE(Tree_erase(&tree, "bar") == TREE_NO_SUCH_KEY);

    {
        std::ostringstream oss;
        REQUIRE(Tree_traverse(&tree, &char_traverse_wrapper, &oss) == TREE_SUCCESS);
        REQUIRE(oss.str() == "baz\nqux\n");
    }

    REQUIRE(Tree_erase(&tree, "baz") == TREE_SUCCESS);
    REQUIRE(Tree_erase(&tree, "baz") == TREE_NO_SUCH_KEY);

    {
        std::ostringstream oss;
        REQUIRE(Tree_traverse(&tree, &char_traverse_wrapper, &oss) == TREE_SUCCESS);
        REQUIRE(oss.str() == "qux\n");
    }

    REQUIRE(Tree_erase(&tree, "qux") == TREE_SUCCESS);
    REQUIRE(Tree_erase(&tree, "qux") == TREE_NO_SUCH_KEY);

    {
        std::ostringstream oss;
        REQUIRE(Tree_traverse(&tree, &char_traverse_wrapper, &oss) == TREE_SUCCESS);
        REQUIRE(oss.str() == "");
    }

    REQUIRE(Tree_destroy(&tree) == TREE_SUCCESS);
}
