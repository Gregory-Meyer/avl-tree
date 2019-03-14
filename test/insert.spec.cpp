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

#include <algorithm>
#include <map>
#include <numeric>
#include <random>
#include <vector>

#include <catch2/catch.hpp>

TEST_CASE("string insertion") {
    avl::Map<std::string, int> map;

    REQUIRE_FALSE(map.insert("foo", 5));
    REQUIRE(map.insert("foo", 5));

    REQUIRE_FALSE(map.insert("bar", 10));
    REQUIRE(map.insert("foo", 5));
    REQUIRE(map.insert("bar", 10));

    REQUIRE_FALSE(map.insert("baz", 15));
    REQUIRE(map.insert("foo", 5));
    REQUIRE(map.insert("bar", 10));
    REQUIRE(map.insert("baz", 15));

    REQUIRE_FALSE(map.insert("qux", 20));
    REQUIRE(map.insert("foo", 5));
    REQUIRE(map.insert("bar", 10));
    REQUIRE(map.insert("baz", 15));
    REQUIRE(map.insert("qux", 20));
}

constexpr int NUM_INSERTIONS = 512;

TEST_CASE("sorted insertion") {
    avl::Map<int, int> map;
    std::vector<int> inserted;

    for (int i = 0; i < NUM_INSERTIONS; ++i) {
        REQUIRE_FALSE(map.insert(i, i));
        inserted.push_back(i);

        for (int j : inserted) {
            REQUIRE(map.insert(j, j));
        }
    }
}

TEST_CASE("random insertion") {
    avl::Map<int, int> map;

    std::vector<int> to_insert(NUM_INSERTIONS);
    std::iota(to_insert.begin(), to_insert.end(), 0);
    std::shuffle(to_insert.begin(), to_insert.end(), std::mt19937());

    std::vector<int> inserted;

    for (int i : to_insert) {
        REQUIRE_FALSE(map.insert(i, i));
        inserted.push_back(i);

        for (int j : inserted) {
            REQUIRE(map.insert(j, j));
        }
    }
}
