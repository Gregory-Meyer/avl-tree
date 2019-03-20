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

#include <vector>

#include <catch2/catch.hpp>

constexpr const char *const LONG_STRING = "this string is so long that it certainly wouldn't be SSO";

TEST_CASE("insert or assign") {
    avl::Map<std::string, int> map;

    std::string s(LONG_STRING);
    REQUIRE(map.insert_or_assign(std::move(s), 42).second);
    REQUIRE(s.empty()); // moved from
    s = LONG_STRING;
    REQUIRE_FALSE(map.insert_or_assign(std::move(s), 42).second);
    REQUIRE(s == LONG_STRING); // not moved from
}

constexpr std::size_t NUM_INSERTIONS = 2048;

TEST_CASE("insert or assign, sorted insertion + sorted assignment") {
    avl::Map<int, int> map;
    const std::vector<int> to_insert = iota(NUM_INSERTIONS);

    for (int i : to_insert) {
        REQUIRE(map.insert_or_assign(i, i).second);
    }

    for (int i : to_insert) {
        const auto ret = map.insert_or_assign(i, i * 2);

        REQUIRE_FALSE(ret.second);
        REQUIRE(ret.first.second == i * 2);
    }
}

TEST_CASE("insert or assign, sorted insertion + random assignment") {
    avl::Map<int, int> map;
    std::vector<int> to_insert = iota(NUM_INSERTIONS);

    for (int i : to_insert) {
        REQUIRE(map.insert_or_assign(i, i).second);
    }

    const auto urbg_ptr = make_urbg();
    to_insert = shuffled(std::move(to_insert), *urbg_ptr);

    for (int i : to_insert) {
        const auto ret = map.insert_or_assign(i, i * 2);

        REQUIRE_FALSE(ret.second);
        REQUIRE(ret.first.second == i * 2);
    }
}

TEST_CASE("insert or assign, random insertion + sorted assignment") {
    avl::Map<int, int> map;
    const auto urbg_ptr = make_urbg();
    std::vector<int> to_insert = rand_iota(NUM_INSERTIONS, *urbg_ptr);

    for (int i : to_insert) {
        REQUIRE(map.insert_or_assign(i, i).second);
    }

    to_insert = sorted(std::move(to_insert));

    for (int i : to_insert) {
        const auto ret = map.insert_or_assign(i, i * 2);

        REQUIRE_FALSE(ret.second);
        REQUIRE(ret.first.second == i * 2);
    }
}

TEST_CASE("insert or assign, random insertion + random assignment") {
    avl::Map<int, int> map;
    const auto urbg_ptr = make_urbg();
    std::vector<int> to_insert = rand_iota(NUM_INSERTIONS, *urbg_ptr);

    for (int i : to_insert) {
        REQUIRE(map.insert_or_assign(i, i).second);
    }

    to_insert = shuffled(std::move(to_insert), *urbg_ptr);

    for (int i : to_insert) {
        const auto ret = map.insert_or_assign(i, i * 2);

        REQUIRE_FALSE(ret.second);
        REQUIRE(ret.first.second == i * 2);
    }
}
