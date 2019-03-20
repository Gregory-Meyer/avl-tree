//  MIT License
//
//  Copyright (c) 2019 Gregory Meyer
//
//  Permission is hereby granted, free of charge, to any person
//  obtaining a copy of this software and associated documentation
//  files (the "Software"), to deal in the Software without
//  restriction, including without limitation the rights to use, copy,
//  modify, merge, publish, distribute, sublicense, and/or sell copies
//  of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice (including
//  the next paragraph) shall be included in all copies or substantial
//  portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
//  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
//  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
//  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
//  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
//  ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
//  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
//  IN THE SOFTWARE.

#include "avl_map.h"
#include "util.h"

#include <algorithm>
#include <map>
#include <numeric>
#include <random>
#include <vector>

#include <catch2/catch.hpp>

TEST_CASE("string insertion") {
    avl::Map<std::string, int> map;

    REQUIRE_FALSE(map.insert("foo", 5).second);
    REQUIRE(map.insert("foo", 5).second);

    REQUIRE_FALSE(map.insert("bar", 10).second);
    REQUIRE(map.insert("foo", 5).second);
    REQUIRE(map.insert("bar", 10).second);

    REQUIRE_FALSE(map.insert("baz", 15).second);
    REQUIRE(map.insert("foo", 5).second);
    REQUIRE(map.insert("bar", 10).second);
    REQUIRE(map.insert("baz", 15).second);

    REQUIRE_FALSE(map.insert("qux", 20).second);
    REQUIRE(map.insert("foo", 5).second);
    REQUIRE(map.insert("bar", 10).second);
    REQUIRE(map.insert("baz", 15).second);
    REQUIRE(map.insert("qux", 20).second);
}

constexpr int NUM_INSERTIONS = 2048;

TEST_CASE("sorted insertion") {
    avl::Map<int, int> map;
    std::vector<int> inserted;

    for (int i = 0; i < NUM_INSERTIONS; ++i) {
        REQUIRE_FALSE(map.insert(i, i).second);
        inserted.push_back(i);

        for (int j : inserted) {
            REQUIRE(map.insert(j, j).second);
        }
    }
}

TEST_CASE("random insertion") {
    avl::Map<int, int> map;

    const auto urbg_ptr = make_urbg();
    const std::vector<int> to_insert = rand_iota(NUM_INSERTIONS, *urbg_ptr);

    std::vector<int> inserted;

    for (int i : to_insert) {
        REQUIRE_FALSE(map.insert(i, i).second);
        inserted.push_back(i);

        for (int j : inserted) {
            REQUIRE(map.insert(j, j).second);
        }
    }
}
